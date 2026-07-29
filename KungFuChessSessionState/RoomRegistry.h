#pragma once

#include "SessionId.h"

#include <mutex>
#include <string>
#include <unordered_map>

// The dependency-free bookkeeping half of room management: generating
// fresh room ids and tracking which room (if any) each connection
// currently belongs to. The heavier half - actually owning a GameSession/
// PlayerAssignment/GameEndCoordinator per room - lives in
// KungFuChessServer's Room/RoomManager, which pull in Core/Network/
// Accounts dependencies this class deliberately avoids, so RoomRegistry
// can be unit-tested here the same way PlayerAssignment/PlayerDirectory
// are.
// Thread-safe: same precedent as PlayerAssignment/PlayerDirectory -
// callers may be on the network I/O thread (disconnect cleanup) or the
// game thread (room creation/joining).
class RoomRegistry
{
public:
    // Returns a fresh, never-before-used room id ("1", "2", "3", ...).
    // The only place room ids are generated - swapping to random,
    // human-shareable codes later only touches this one method.
    std::string createRoomId();

    // Which room (if any) a connection currently belongs to. Returns an
    // empty string if none (never joined, or already left/disconnected).
    std::string roomIdForSession(SessionId id) const;
    void setSessionRoom(SessionId id, const std::string& roomId);
    void clearSessionRoom(SessionId id);

private:
    mutable std::mutex m_mutex;
    int m_nextRoomNumber = 1;
    std::unordered_map<SessionId, std::string> m_sessionRooms;
};
