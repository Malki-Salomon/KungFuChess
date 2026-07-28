#include "Server.h"

#include "GameSession.h"
#include "../Protocol/MoveTranslator.h"
#include "LoginMessage.h"

#include <chrono>
#include <string>

namespace
{
    constexpr unsigned short kListenPort = 9000; // placeholder - move to config later

    // The only place that decides what a seat *means* in this specific
    // game. PlayerAssignment itself knows nothing about chess/colors - see
    // its header - so that mapping lives here instead, in a file that
    // already legitimately depends on Core.
    PieceColor colorForSeat(Seat seat)
    {
        switch (seat)
        {
            case Seat::First:  return PieceColor::White;
            case Seat::Second: return PieceColor::Black;
            default:           return PieceColor::None; // spectator
        }
    }

    std::string colorName(PieceColor color)
    {
        switch (color)
        {
            case PieceColor::White: return "white";
            case PieceColor::Black: return "black";
            default:                return "spectator";
        }
    }
}

Server::Server()
    : m_webSocketServer(kListenPort)
    , m_snapshotBroadcaster(m_webSocketServer)
    , m_running(true)
{
    // First connection -> white, second -> black, everyone else ->
    // spectator. Tell the new connection what it got.
    m_webSocketServer.setConnectHandler([this](SessionId id)
    {
        PieceColor assigned = colorForSeat(m_playerAssignment.assign(id));
        m_webSocketServer.sendTo(id, "{\"type\":\"assigned\",\"color\":\"" + colorName(assigned) + "\"}");
    });

    // Network thread -> for a login message, record the username directly
    // (PlayerDirectory is mutex-protected, same precedent as
    // PlayerAssignment's assign()/release() above, both already called
    // straight from these I/O-thread handlers). Anything else just gets
    // queued; never touches Game/App directly (see CommandInbox for why).
    m_webSocketServer.setMessageHandler([this](SessionId id, const std::string& text)
    {
        std::string username;
        if (LoginMessage::parse(text, username))
        {
            m_playerDirectory.set(id, username);
            return;
        }

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

    while (m_running)
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
                PieceColor senderColor = colorForSeat(m_playerAssignment.seatOf(inbound.sender));
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
    }
}
