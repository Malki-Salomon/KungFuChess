#include "PlayerAssignment.h"

PlayerAssignment::PlayerAssignment() = default;

Seat PlayerAssignment::assign(SessionId id)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Idempotent: a second assign() for a session that already holds a
    // seat (e.g. a connection that logs in more than once) must return
    // that same seat unchanged, not potentially reassign/demote it.
    auto existing = m_assignments.find(id);
    if (existing != m_assignments.end())
        return existing->second;

    Seat seat;
    if (!m_first.has_value())
    {
        m_first = id;
        seat = Seat::First;
    }
    else if (!m_second.has_value())
    {
        m_second = id;
        seat = Seat::Second;
    }
    else
    {
        seat = Seat::Spectator;
    }

    m_assignments[id] = seat;
    return seat;
}

void PlayerAssignment::release(SessionId id)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_first.has_value() && m_first.value() == id)
        m_first.reset();
    if (m_second.has_value() && m_second.value() == id)
        m_second.reset();

    m_assignments.erase(id);
}

Seat PlayerAssignment::seatOf(SessionId id) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_assignments.find(id);
    if (it == m_assignments.end())
        return Seat::Spectator;

    return it->second;
}

SessionId PlayerAssignment::sessionIdForSeat(Seat seat) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    switch (seat)
    {
        case Seat::First:  return m_first.has_value() ? m_first.value() : kInvalidSessionId;
        case Seat::Second: return m_second.has_value() ? m_second.value() : kInvalidSessionId;
        default:           return kInvalidSessionId;
    }
}
