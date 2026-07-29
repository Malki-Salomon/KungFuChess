#include "Server.h"

#include "GameSession.h"
#include "SeatColorNaming.h"
#include "../Protocol/MoveTranslator.h"
#include "LoginMessage.h"
#include "RegisterMessage.h"

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
    , m_snapshotBroadcaster(m_webSocketServer)
    , m_userRepository(kUserDbPath)
    , m_authService(m_userRepository)
    , m_authWorker(m_authService, m_webSocketServer, m_playerDirectory, m_playerAssignment)
    , m_matchResultService(m_userRepository)
    , m_gameEndCoordinator(*m_sessionManager.getPrimarySession(), m_playerAssignment, m_playerDirectory, m_matchResultService, m_webSocketServer)
    , m_running(true)
{
    // Ctrl+C (or an equivalent SIGINT) is the only way this process ever
    // stops today - see run()'s loop condition and its closing comment.
    std::signal(SIGINT, handleShutdownSignal);

    // Strict rule: a connection that hasn't successfully logged in is, to
    // the rest of the system, as if it doesn't exist. broadcast() is the
    // one place that gets enforced centrally (see WebSocketServer.h) so
    // every current and future broadcaster (SnapshotBroadcaster,
    // GameEndCoordinator, ...) gets this correctly without having to
    // remember to filter itself.
    m_webSocketServer.setAuthenticationCheck([this](SessionId id)
    {
        return m_playerDirectory.isLoggedIn(id);
    });

    // No seat assignment or messages here anymore - a connection is
    // "nobody" until it logs in (AuthWorker does the seat assignment +
    // "assigned" message + board catch-up, all three, at that point -
    // see AuthWorker.cpp). Kept as a no-op hook only for connect-time
    // visibility; WebSocketServer's own Session::run() already logs the
    // raw handshake to stdout.
    m_webSocketServer.setConnectHandler([](SessionId id)
    {
        std::cout << "[Server] connection " << id << " established (not yet authenticated)\n";
    });

    // Network thread -> for a register/login message, do the absolute
    // minimum here (recognize the type, parse the fields, hand off) and
    // return immediately. The actual work - AuthService, meaning real
    // Argon2id hashing (stage 4c) and real SQLite disk I/O (stage 4b) -
    // happens on AuthWorker's own dedicated thread, not here. Before
    // stage 4d, this ran AuthService synchronously right in this
    // callback, which stalled every other connected client's reads/
    // writes for as long as one hash/disk call took - see
    // KungFuChessAccounts/README.md and AuthWorker.h for the full story.
    // AuthWorker does everything a successful login triggers itself (see
    // its own comment), so none of that happens here either.
    // Anything else - if the sender isn't logged in yet - is ignored
    // outright: not queued, not processed, not even reaching the
    // ownership check in run() below. This is the crux of "gets nothing
    // until login": only a logged-in sender's non-auth messages ever
    // reach CommandInbox.
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

    // Free the seat so a later connection can take it.
    m_webSocketServer.setDisconnectHandler([this](SessionId id)
    {
        m_playerAssignment.release(id);
        m_playerDirectory.release(id);
    });

    // POC: every connected client's messages go to the one session that
    // exists today, and that session's board changes get broadcast back to
    // every connected client. Revisit once sessions map to connections.
    if (GameSession* primary = m_sessionManager.getPrimarySession())
    {
        primary->attachPrinter(&m_snapshotBroadcaster);
    }
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

        if (GameSession* primary = m_sessionManager.getPrimarySession())
        {
            for (const auto& inbound : m_commandInbox.drainAll())
            {
                ParsedMove move = MoveTranslator::parseMove(inbound.text);
                if (!move.valid)
                    continue;

                // No turns in this game (real-time "kung fu chess"), but a
                // connection may only ever move pieces of its own assigned
                // color. Spectators can't move anything. Queried straight
                // from Core (via GameSession) rather than from a cache -
                // SnapshotBroadcaster's job is only to broadcast, not to
                // answer questions about board state.
                //
                // Direct indexing is safe here without a bounds check:
                // MoveNotation::squareToPosition only ever produces 0..7 for
                // a valid square (file a-h, rank 1-8), and move.valid is
                // already checked above - it's the earlier guard that
                // guarantees this invariant, not anything about the
                // snapshot itself.
                PieceColor senderColor = SeatColorNaming::colorForSeat(m_playerAssignment.seatOf(inbound.sender));
                GameSnapshot snapshot = primary->getSnapshot();
                PieceColor pieceColorAtFrom = snapshot.cells[move.fromRow][move.fromCol].color;

                if (senderColor == PieceColor::None || senderColor != pieceColorAtFrom)
                    continue; // silently ignored, same as Core's own illegal-move handling

                for (const auto& coreCmd : MoveTranslator::toClickCommands(move))
                {
                    primary->dispatchCommand(coreCmd);
                }
            }

            if (deltaMs > 0)
            {
                primary->dispatchCommand("wait " + std::to_string(deltaMs));
                // Only advance the baseline by what we actually reported -
                // this loop runs with no sleep, so most individual passes
                // take far under 1ms. Resetting prevTime every iteration
                // (the previous bug) discarded almost all elapsed time
                // instead of letting those sub-millisecond gaps accumulate
                // into real "wait" ticks.
                prevTime = currentTime;
            }
        }

        m_sessionManager.tickAllSessions();
        m_gameEndCoordinator.checkAndHandle();
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
