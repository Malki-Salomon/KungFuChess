#pragma once

#include "PlayerAssignment.h" // for Seat

#include <chrono>
#include <mutex>
#include <optional>
#include <string>

// What's remembered about a player-seat disconnect while its reconnect
// grace period is running: which seat, whose username (so a reconnecting
// connection can be matched by identity, not by session id - the id that
// disconnected is gone for good), and when the grace period expires.
struct PendingDisconnect
{
    Seat seat;
    std::string username;
    std::chrono::steady_clock::time_point deadline;
};

// Tracks at most one pending disconnect at a time (simultaneous/
// cascading disconnects just let the existing timer expire normally -
// see Room, the one owner of an instance of this). Deliberately
// dependency-free - just Seat, a username, and a deadline - so it can be
// unit-tested here the same way PlayerAssignment/PlayerDirectory/
// RoomRegistry are; the heavier Room class that owns one of these pulls
// in Core/Network/Accounts and can't be.
// Thread-safe: begin()/hasPending()/pending()/clear()/tryReclaim() may
// all be called from the network I/O thread (disconnect) and the game
// thread (join/tick) - same precedent as the rest of this library.
class PendingDisconnectTracker
{
public:
    void begin(Seat seat, const std::string& username, std::chrono::steady_clock::time_point deadline);
    bool hasPending() const;

    // Returned by value rather than by reference - unlike a plain
    // getter, this crosses the mutex boundary, and a reference into
    // internal state wouldn't stay valid once the lock is released.
    // Precondition: hasPending().
    PendingDisconnect pending() const;

    void clear();

    // If a pending disconnect exists AND its username matches, returns
    // the seat to restore into and clears the pending state. Otherwise
    // returns nullopt and changes nothing.
    std::optional<Seat> tryReclaim(const std::string& username);

private:
    mutable std::mutex m_mutex;
    std::optional<PendingDisconnect> m_pending;
};
