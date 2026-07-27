#pragma once

#include "../Network/SessionId.h"
#include "PieceTypes.h" // Core's PieceColor enum - White/Black/None.
                        // Reused directly: None doubles as "spectator"
                        // here, since it can never equal an actual piece's
                        // color, which is exactly what we want when
                        // checking move ownership later.

#include <mutex>
#include <optional>
#include <unordered_map>

// Assigns the first connection White, the second Black, and treats every
// later connection as a spectator (represented as PieceColor::None).
// Thread-safe: assign()/release()/colorOf() may all be called from the
// network I/O thread and the game thread.
class PlayerAssignment
{
public:
    PlayerAssignment();

    // Called once per new connection. Returns the seat assigned.
    PieceColor assign(SessionId id);

    // Called on disconnect. Frees the seat if this session held one, so a
    // later connection can take it. No-op for a spectator or unknown id.
    void release(SessionId id);

    // Returns the color previously assigned to this session, or
    // PieceColor::None if it was a spectator or isn't tracked (e.g. already
    // released).
    PieceColor colorOf(SessionId id) const;

private:
    mutable std::mutex m_mutex;
    std::optional<SessionId> m_white;
    std::optional<SessionId> m_black;
    std::unordered_map<SessionId, PieceColor> m_assignments;
};
