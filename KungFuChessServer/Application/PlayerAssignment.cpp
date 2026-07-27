#include "PlayerAssignment.h"

PlayerAssignment::PlayerAssignment() = default;

PieceColor PlayerAssignment::assign(SessionId id)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    PieceColor color;
    if (!m_white.has_value())
    {
        m_white = id;
        color = PieceColor::White;
    }
    else if (!m_black.has_value())
    {
        m_black = id;
        color = PieceColor::Black;
    }
    else
    {
        color = PieceColor::None; // spectator
    }

    m_assignments[id] = color;
    return color;
}

void PlayerAssignment::release(SessionId id)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_white.has_value() && m_white.value() == id)
        m_white.reset();
    if (m_black.has_value() && m_black.value() == id)
        m_black.reset();

    m_assignments.erase(id);
}

PieceColor PlayerAssignment::colorOf(SessionId id) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_assignments.find(id);
    if (it == m_assignments.end())
        return PieceColor::None;

    return it->second;
}
