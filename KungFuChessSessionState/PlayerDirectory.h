#pragma once

// SessionId.h stays put in KungFuChessServer/Network (it's the network
// layer's connection-identity type) - this project just adds it to its
// include path, same as PlayerAssignment.h in this same project.
#include "SessionId.h"

#include <mutex>
#include <string>
#include <unordered_map>

// Remembers which username each connection logged in with. Deliberately
// separate from PlayerAssignment (seat/color) - a username and a seat are
// two unrelated facts about a connection, tracked independently per SRP.
// Thread-safe: set()/usernameOf()/isLoggedIn()/release() may all be
// called from the network I/O thread and the game thread.
class PlayerDirectory
{
public:
    void set(SessionId id, const std::string& username);

    // Returns an empty string if this session never logged in (or already
    // disconnected).
    std::string usernameOf(SessionId id) const;

    // True iff a username is currently set for this id - i.e. this
    // connection has successfully logged in and hasn't disconnected
    // since. The one place the rest of the system asks "is this
    // connection allowed to do anything yet?".
    bool isLoggedIn(SessionId id) const;

    void release(SessionId id);

private:
    mutable std::mutex m_mutex;
    std::unordered_map<SessionId, std::string> m_usernames;
};
