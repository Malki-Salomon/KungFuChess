#pragma once

#include "SessionId.h"

#include <mutex>
#include <string>
#include <vector>

// WebSocketServer's message callback fires on its own I/O thread, but
// GameSession/Game are only safe to touch from the single thread that ticks
// them (see Server::run()). This queue is the hand-off point: the network
// thread pushes (sender, raw command text) here, and the game thread drains
// it once per tick before ticking sessions - so Core is only ever touched
// from one thread. The sender id travels along so ownership can be checked
// (e.g. "is this connection actually allowed to move this piece") before
// anything gets dispatched into Core.
struct InboundCommand
{
    SessionId sender;
    std::string text;
};

class CommandInbox
{
public:
    void push(SessionId sender, const std::string& text)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pending.push_back({ sender, text });
    }

    // Returns everything queued since the last call, and clears the queue.
    std::vector<InboundCommand> drainAll()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<InboundCommand> drained;
        drained.swap(m_pending);
        return drained;
    }

private:
    std::mutex m_mutex;
    std::vector<InboundCommand> m_pending;
};
