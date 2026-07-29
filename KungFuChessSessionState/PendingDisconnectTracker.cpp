#include "PendingDisconnectTracker.h"

void PendingDisconnectTracker::begin(Seat seat, const std::string& username, std::chrono::steady_clock::time_point deadline)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pending = PendingDisconnect{ seat, username, deadline };
}

bool PendingDisconnectTracker::hasPending() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_pending.has_value();
}

PendingDisconnect PendingDisconnectTracker::pending() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return *m_pending;
}

void PendingDisconnectTracker::clear()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pending.reset();
}

std::optional<Seat> PendingDisconnectTracker::tryReclaim(const std::string& username)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_pending.has_value() || m_pending->username != username)
        return std::nullopt;

    Seat seat = m_pending->seat;
    m_pending.reset();
    return seat;
}
