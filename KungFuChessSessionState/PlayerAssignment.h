#pragma once

// SessionId.h stays put in KungFuChessServer/Network (it's the network
// layer's connection-identity type) - this project just adds it to its
// include path, same as KungFuChessAccounts pulling in Core-independent
// headers from wherever they legitimately live.
#include "SessionId.h"

#include <mutex>
#include <optional>
#include <unordered_map>

// Which of the (at most two) seats a connection holds. Deliberately knows
// nothing about chess/colors - that mapping (First -> White, Second ->
// Black) belongs to whichever caller cares about the game, not here. This
// class only tracks connection order.
enum class Seat
{
    First,
    Second,
    Spectator
};

// Assigns the first connection Seat::First, the second Seat::Second, and
// treats every later connection as Seat::Spectator.
// Thread-safe: assign()/release()/seatOf() may all be called from the
// network I/O thread and the game thread.
class PlayerAssignment
{
public:
    PlayerAssignment();

    // Called once per new connection. Returns the seat assigned.
    Seat assign(SessionId id);

    // Called on disconnect. Frees the seat if this session held one, so a
    // later connection can take it. No-op for a spectator or unknown id.
    void release(SessionId id);

    // Returns the seat previously assigned to this session, or
    // Seat::Spectator if it was a spectator or isn't tracked (e.g. already
    // released).
    Seat seatOf(SessionId id) const;

    // The reverse of seatOf(): which SessionId (if any) currently holds
    // this seat. Returns kInvalidSessionId for Seat::Spectator (many
    // connections can hold that "seat" at once, so there's no single
    // answer) or for an empty First/Second seat.
    SessionId sessionIdForSeat(Seat seat) const;

private:
    mutable std::mutex m_mutex;
    std::optional<SessionId> m_first;
    std::optional<SessionId> m_second;
    std::unordered_map<SessionId, Seat> m_assignments;
};
