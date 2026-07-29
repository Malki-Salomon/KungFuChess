#include "Server.h"

#include "Room.h"
#include "GameSession.h"
#include "PlayerAssignment.h"
#include "SeatColorNaming.h"
#include "../Protocol/MoveTranslator.h"
#include "../Protocol/SnapshotBroadcaster.h"
#include "LoginMessage.h"
#include "RegisterMessage.h"
#include "CreateRoomMessage.h"
#include "JoinRoomMessage.h"
#include "RoomCreatedMessage.h"
#include "RoomJoinedMessage.h"
#include "RoomJoinFailedMessage.h"
#include "AssignedMessage.h"
#include "OpponentDisconnectedMessage.h"
#include "OpponentReconnectedMessage.h"
#include "Logger.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <string>

namespace
{
    constexpr unsigned short kListenPort = 9000; // placeholder - move to config later

    // Fixed relative path next to the executable - no config system
    // exists yet (see KungFuChessServer/Infrastructure/README.md), same
    // "placeholder constant" status as kListenPort above.
    constexpr const char* kUserDbPath = "kungfuchess.db";

    // How long a disconnected player's seat is held in reserve before
    // the room's other player wins by resignation. Shared by the
    // deadline computed in the disconnect handler and the
    // "secondsRemaining" told to the opponent, so the two can't drift.
    constexpr int kReconnectGraceSeconds = 20;

    // Set by handleShutdownSignal() - a real OS signal handler, which per
    // the C++ standard may only touch a very restricted set of things
    // (lock-free std::atomic access is explicitly one of them). Anything
    // heavier - calling WebSocketServer::stop(), joining AuthWorker's
    // thread, etc. - is NOT safe to do directly from signal context, so
    // it happens in Server::run() instead, which just polls this flag on
    // an ordinary thread.
    std::atomic<bool> g_shutdownRequested{ false };

    void handleShutdownSignal(int)
    {
        g_shutdownRequested.store(true);
    }
}

Server::Server()
    : m_webSocketServer(kListenPort)
    , m_userRepository(kUserDbPath)
    , m_authService(m_userRepository)
    , m_authWorker(m_authService, m_webSocketServer, m_playerDirectory)
    , m_matchResultService(m_userRepository)
    , m_roomManager(m_playerDirectory, m_matchResultService, m_webSocketServer)
    , m_running(true)
{
    // Ctrl+C (or an equivalent SIGINT) is the only way this process ever
    // stops today - see run()'s loop condition and its closing comment.
    std::signal(SIGINT, handleShutdownSignal);

    // No seat/room assignment here - a connection is "nobody" until it
    // logs in AND joins a room (see the message handler below and
    // run()'s createRoom/joinRoom handling). Kept as a no-op hook only
    // for connect-time visibility; WebSocketServer's own Session::run()
    // already logs the raw handshake to stdout.
    m_webSocketServer.setConnectHandler([](SessionId id)
    {
        Logger::info("[Server] connection " + std::to_string(id) + " established (not yet authenticated)");
    });

    // Network thread -> for a register/login message, do the absolute
    // minimum here (recognize the type, parse the fields, hand off) and
    // return immediately. The actual work - AuthService, meaning real
    // Argon2id hashing (stage 4c) and real SQLite disk I/O (stage 4b) -
    // happens on AuthWorker's own dedicated thread, not here.
    // AuthWorker sends the authResult itself and populates PlayerDirectory
    // on a successful login, so neither happens here either.
    // Anything else - if the sender isn't logged in yet - is ignored
    // outright: not queued, not processed. This is the crux of "gets
    // nothing until login": only a logged-in sender's non-auth messages
    // (createRoom/joinRoom/move alike) ever reach CommandInbox, to be
    // routed by run() on the game thread - creating/joining/touching a
    // Room must only ever happen there, same existing rule as moves.
    m_webSocketServer.setMessageHandler([this](SessionId id, const std::string& text)
    {
        std::string regUsername, regPassword;
        if (RegisterMessage::parse(text, regUsername, regPassword))
        {
            m_authWorker.push({ id, AuthRequestType::Register, regUsername, regPassword });
            return;
        }

        std::string loginUsername, loginPassword;
        if (LoginMessage::parse(text, loginUsername, loginPassword))
        {
            m_authWorker.push({ id, AuthRequestType::Login, loginUsername, loginPassword });
            return;
        }

        if (!m_playerDirectory.isLoggedIn(id))
            return;

        m_commandInbox.push(id, text);
    });

    // A disconnecting SPECTATOR, or a player whose room's game has
    // already ended, gets today's immediate cleanup - nothing left to
    // hold open for. A player-seat disconnect mid-game is different:
    // their seat is held in reserve for a grace period instead of being
    // released immediately, so a real network hiccup doesn't
    // automatically forfeit the game - see beginPendingDisconnect(),
    // the joinRoom reclaim check below, and run()'s timeout finalization.
    m_webSocketServer.setDisconnectHandler([this](SessionId id)
    {
        if (Room* room = m_roomManager.roomForSession(id))
        {
            Seat seat = room->playerAssignment().seatOf(id);
            bool gameInProgress = room->session().getStatus() == GameStatus::Playing;

            if (seat != Seat::Spectator && gameInProgress)
            {
                std::string username = m_playerDirectory.usernameOf(id);
                room->beginPendingDisconnect(seat, username,
                    std::chrono::steady_clock::now() + std::chrono::seconds(kReconnectGraceSeconds));
                room->removeMember(id);
                m_webSocketServer.sendToMany(room->members(), OpponentDisconnectedMessage::build(kReconnectGraceSeconds));
                Logger::info("[Server] player \"" + username + "\" disconnected mid-game in room " + room->id()
                    + " - " + std::to_string(kReconnectGraceSeconds) + "s to reconnect before resignation");
                return;
            }
        }

        Logger::info("[Server] connection " + std::to_string(id) + " disconnected");
        leaveCurrentRoom(id);
        m_playerDirectory.release(id);
    });
}

void Server::leaveCurrentRoom(SessionId id)
{
    if (Room* room = m_roomManager.roomForSession(id))
    {
        room->removeMember(id);
        room->playerAssignment().release(id);
    }
    m_roomManager.clearSessionRoom(id);
}

void Server::run()
{
    m_webSocketServer.start();

    // Same pattern as the GUI's GameLoop::run(): real-time piece movement
    // is driven by RealTimeArbiter, which only advances when it receives
    // "wait <ms>" commands. Without feeding those in continuously (like the
    // GUI does every frame), nothing here would ever actually move, no
    // matter what commands a client sends.
    auto prevTime = std::chrono::steady_clock::now();

    while (m_running && !g_shutdownRequested.load())
    {
        auto currentTime = std::chrono::steady_clock::now();
        auto deltaMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - prevTime).count();

        for (const auto& inbound : m_commandInbox.drainAll())
        {
            if (CreateRoomMessage::parse(inbound.text))
            {
                std::string code = m_roomManager.createRoom();
                m_webSocketServer.sendTo(inbound.sender, RoomCreatedMessage::build(code));
                Logger::info("[Server] room " + code + " created by connection " + std::to_string(inbound.sender));
                continue;
            }

            std::string joinCode;
            if (JoinRoomMessage::parse(inbound.text, joinCode))
            {
                Room* room = m_roomManager.findRoom(joinCode);
                if (!room)
                {
                    m_webSocketServer.sendTo(inbound.sender, RoomJoinFailedMessage::build("room not found"));
                    continue;
                }

                // A connection switching rooms must not leave stale
                // membership behind in whatever room it was in before -
                // same "minimal hygiene" reasoning as disconnect cleanup.
                leaveCurrentRoom(inbound.sender);

                // A reconnecting player - same username, same room code,
                // within the grace period - gets restored to their exact
                // old seat instead of a fresh assignment. No new client
                // behavior needed: this is recognized purely from the
                // server side of an ordinary login -> joinRoom sequence.
                std::string username = m_playerDirectory.usernameOf(inbound.sender);
                if (auto reclaimedSeat = room->tryReclaim(username))
                {
                    // Tell the room's OTHER member(s) before adding the
                    // reconnecting player back into members() - they
                    // must not be included in their own "you're back"
                    // notice.
                    m_webSocketServer.sendToMany(room->members(), OpponentReconnectedMessage::build());

                    room->playerAssignment().reoccupySeat(*reclaimedSeat, inbound.sender);
                    room->addMember(inbound.sender);
                    m_roomManager.setSessionRoom(inbound.sender, joinCode);

                    std::string reclaimedRole = (*reclaimedSeat == Seat::Spectator) ? "spectator" : "player";
                    m_webSocketServer.sendTo(inbound.sender, RoomJoinedMessage::build(joinCode, reclaimedRole));

                    PieceColor reclaimedColor = SeatColorNaming::colorForSeat(*reclaimedSeat);
                    m_webSocketServer.sendTo(inbound.sender, AssignedMessage::build(SeatColorNaming::colorName(reclaimedColor)));

                    m_webSocketServer.sendTo(inbound.sender, SnapshotBroadcaster::buildBoardMessage(room->session().getSnapshot()));
                    Logger::info("[Server] \"" + username + "\" reconnected to room " + joinCode);
                    continue;
                }

                Seat seat = room->playerAssignment().assign(inbound.sender);
                room->addMember(inbound.sender);
                m_roomManager.setSessionRoom(inbound.sender, joinCode);

                std::string role = (seat == Seat::Spectator) ? "spectator" : "player";
                m_webSocketServer.sendTo(inbound.sender, RoomJoinedMessage::build(joinCode, role));

                PieceColor color = SeatColorNaming::colorForSeat(seat);
                m_webSocketServer.sendTo(inbound.sender, AssignedMessage::build(SeatColorNaming::colorName(color)));

                // Immediate catch-up on this room's current board - this
                // connection received nothing before now (see the strict
                // login-gating stage), so without this it would see a
                // blank/frozen board until the next unrelated move in
                // this room happens to broadcast one.
                m_webSocketServer.sendTo(inbound.sender, SnapshotBroadcaster::buildBoardMessage(room->session().getSnapshot()));
                Logger::info("[Server] connection " + std::to_string(inbound.sender) + " joined room " + joinCode + " as " + role);
                continue;
            }

            // Not a room-management message - try it as a move, routed
            // to the sender's current room (if any). A sender with no
            // room is a no-op, same as an unassigned/spectator sender
            // always was.
            Room* room = m_roomManager.roomForSession(inbound.sender);
            if (!room)
                continue;

            // A room isn't playable until it actually has two players -
            // the sole occupant of a fresh room has no opponent yet, so
            // moves are a no-op (same silent-ignore pattern as every
            // other rejected-move case below) until a second player
            // joins. Doesn't affect this room's wait/tick dispatch below
            // - RealTimeArbiter is harmless to keep ticking with nothing
            // queued while waiting for that second player.
            if (!room->playerAssignment().bothSeatsFilled())
                continue;

            ParsedMove move = MoveTranslator::parseMove(inbound.text);
            if (!move.valid)
                continue;

            // No turns in this game (real-time "kung fu chess"), but a
            // connection may only ever move pieces of its own assigned
            // color, within its own room. Spectators can't move
            // anything. Queried straight from Core (via GameSession)
            // rather than from a cache - SnapshotBroadcaster's job is
            // only to broadcast, not to answer questions about board
            // state.
            //
            // Direct indexing is safe here without a bounds check:
            // MoveNotation::squareToPosition only ever produces 0..7 for
            // a valid square (file a-h, rank 1-8), and move.valid is
            // already checked above - it's the earlier guard that
            // guarantees this invariant, not anything about the
            // snapshot itself.
            PieceColor senderColor = SeatColorNaming::colorForSeat(room->playerAssignment().seatOf(inbound.sender));
            GameSnapshot snapshot = room->session().getSnapshot();
            PieceColor pieceColorAtFrom = snapshot.cells[move.fromRow][move.fromCol].color;

            if (senderColor == PieceColor::None || senderColor != pieceColorAtFrom)
                continue; // silently ignored, same as Core's own illegal-move handling

            for (const auto& coreCmd : MoveTranslator::toClickCommands(move))
            {
                room->session().dispatchCommand(coreCmd);
            }
        }

        // Every active room gets its own real-time tick and its own
        // game-end check - replaces the single "primary session" this
        // loop used to operate on before rooms existed.
        for (const auto& room : m_roomManager.allRooms())
        {
            if (deltaMs > 0)
            {
                room->session().dispatchCommand("wait " + std::to_string(deltaMs));
            }
            room->session().tick();
            room->gameEndCoordinator().checkAndHandle();

            // A pending disconnect whose grace period has expired with
            // no reconnection: the other player wins by resignation -
            // same ratings-update + gameOver flow a real checkmate would
            // produce (see GameEndCoordinator::forceResign()).
            if (room->hasPendingDisconnect())
            {
                PendingDisconnect pending = room->pendingDisconnect();
                if (std::chrono::steady_clock::now() >= pending.deadline)
                {
                    PieceColor loserColor = SeatColorNaming::colorForSeat(pending.seat);
                    PieceColor winnerColor = (loserColor == PieceColor::White) ? PieceColor::Black : PieceColor::White;

                    Logger::info("[Server] room " + room->id() + ": \"" + pending.username
                        + "\" did not reconnect in time - resigning, opponent wins");
                    room->gameEndCoordinator().forceResign(winnerColor);
                    room->clearPendingDisconnect();

                    SessionId staleId = room->playerAssignment().sessionIdForSeat(pending.seat);
                    if (staleId != kInvalidSessionId)
                        room->playerAssignment().release(staleId);
                }
            }
        }

        if (deltaMs > 0)
        {
            // Only advance the baseline by what we actually reported -
            // this loop runs with no sleep, so most individual passes
            // take far under 1ms. Resetting prevTime every iteration
            // (the previous bug) discarded almost all elapsed time
            // instead of letting those sub-millisecond gaps accumulate
            // into real "wait" ticks.
            prevTime = currentTime;
        }
    }

    // Shutdown requested. Deliberately does NOT call m_webSocketServer.
    // stop() here: AuthWorker (declared after m_webSocketServer in
    // Server.h, so it destructs *first*, in reverse declaration order,
    // once this function returns and `Server server;` goes out of scope
    // in main()) needs WebSocketServer's I/O thread still running while
    // it drains its queue and delivers final authResult replies via
    // sendTo() - stopping WebSocketServer before that would make those
    // final sendTo() calls silently no-op (posting onto an io_context
    // whose thread has already stopped just never runs the posted work).
    // ~WebSocketServer() already calls stop() itself, so it's handled
    // once AuthWorker has actually finished, not before.
}
