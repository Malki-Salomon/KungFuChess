#pragma once

#include <mutex>
#include <string>
#include <vector>

// WebSocketServer's message callback fires on its own I/O thread, but
// GameSession/Game are only safe to touch from the single thread that ticks
// them (see Server::run()). This queue is the hand-off point: the network
// thread pushes raw command text here, and the game thread drains it once
// per tick before ticking sessions - so Core is only ever touched from one
// thread.
class CommandInbox
{
public:
    void push(const std::string& text)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pending.push_back(text);
    }

    // Returns everything queued since the last call, and clears the queue.
    std::vector<std::string> drainAll()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<std::string> drained;
        drained.swap(m_pending);
        return drained;
    }

private:
    std::mutex m_mutex;
    std::vector<std::string> m_pending;
};
