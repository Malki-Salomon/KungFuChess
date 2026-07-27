#include "Server.h"

#include "GameSession.h"
#include "../Protocol/MoveTranslator.h"

#include <chrono>
#include <string>

namespace
{
    constexpr unsigned short kListenPort = 9000; // placeholder - move to config later
}

Server::Server()
    : m_webSocketServer(kListenPort)
    , m_snapshotBroadcaster(m_webSocketServer)
    , m_running(true)
{
    // Network thread -> just queues raw text; never touches Game/App
    // directly (see CommandInbox for why).
    m_webSocketServer.setMessageHandler([this](const std::string& text)
    {
        m_commandInbox.push(text);
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

    while (m_running)
    {
        auto currentTime = std::chrono::steady_clock::now();
        auto deltaMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - prevTime).count();

        if (GameSession* primary = m_sessionManager.getPrimarySession())
        {
            for (const auto& raw : m_commandInbox.drainAll())
            {
                for (const auto& coreCmd : MoveTranslator::translateMoveMessage(raw))
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
    }
}
