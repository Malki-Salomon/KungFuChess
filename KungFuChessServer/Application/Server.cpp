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

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <string>

namespace
{
    constexpr unsigned short kListenPort = 9000; // placeholder - move to config later

    // Fixed relative path next to the executable - no config system
    // exists yet (see KungFuChessServer/Infrastructure/README.md), same
    // "placeholder constant" status as kListenPort above.
    constexpr const char* kUserDbPath = "kungfuchess.db";

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
        std::cout << "[Server] connection " << id << " established (not yet authenticated)\n";
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

    // Minimal hygiene only (not the reconnect/countdown feature, which is
    // separate, later work): free this connection's seat and membership
    // in whatever room it was in, so nothing leaks or goes stale.
    m_webSocketServer.setDisconnectHandler([this](SessionId id)
    {
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
