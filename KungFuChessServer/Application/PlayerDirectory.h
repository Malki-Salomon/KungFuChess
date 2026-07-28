#pragma once

#include "../Network/SessionId.h"

#include <mutex>
#include <string>
#include <unordered_map>

// Remembers which username each connection logged in with. Deliberately
// separate from PlayerAssignment (seat/color) - a username and a seat are
// two unrelated facts about a connection, tracked independently per SRP.
// Thread-safe: set()/usernameOf()/release() may all be called from the
// network I/O thread and the game thread.
class PlayerDirectory
{
public:
    void set(SessionId id, const std::string& username);

    // Returns an empty string if this session never logged in (or already
    // disconnected).
    std::string usernameOf(SessionId id) const;

    void release(SessionId id);

private:
    mutable std::mutex m_mutex;
    std::unordered_map<SessionId, std::string> m_usernames;
};
