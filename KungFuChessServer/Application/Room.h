#pragma once

#include "PlayerAssignment.h"
#include "GameEndCoordinator.h"
#include "PendingDisconnectTracker.h"
#include "../Protocol/SnapshotBroadcaster.h"
#include "../Network/SessionId.h"

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class GameSession;
class PlayerDirectory;
class MatchResultService;
class WebSocketServer;

// One active game: bundles what, before rooms existed, used to be spread
// across Server's own members - the GameSession, its own PlayerAssignment
// (seats are a per-game fact, not a global one - see PlayerAssignment.h),
// a SnapshotBroadcaster and GameEndCoordinator scoped to just this room's
// members, and the member list (players AND spectators) those two send
// to via WebSocketServer::sendToMany().
class Room
{
public:
    Room(std::string roomId, std::unique_ptr<GameSession> session,
         PlayerDirectory& playerDirectory, MatchResultService& matchResultService,
         WebSocketServer& webSocketServer);
    ~Room(); // declared here, defined in the .cpp where GameSession is complete

    const std::string& id() const;
    GameSession& session();
    PlayerAssignment& playerAssignment();
    GameEndCoordinator& gameEndCoordinator();

    // Idempotent, like PlayerAssignment::assign() - adding an id already
    // present is a no-op rather than a duplicate entry.
    void addMember(SessionId id);
    void removeMember(SessionId id);
    const std::vector<SessionId>& members() const;

    // Tracks the one player-seat disconnect currently within its
    // reconnect grace period, if any. All five just forward to
    // PendingDisconnectTracker (see its header for the full reasoning -
    // kept dependency-free there so it can be unit-tested, unlike Room
    // itself).
    void beginPendingDisconnect(Seat seat, const std::string& username, std::chrono::steady_clock::time_point deadline);
    bool hasPendingDisconnect() const;
    PendingDisconnect pendingDisconnect() const; // precondition: hasPendingDisconnect()
    void clearPendingDisconnect();
    std::optional<Seat> tryReclaim(const std::string& username);

private:
    std::string m_id;
    std::unique_ptr<GameSession> m_session;
    PlayerAssignment m_playerAssignment;
    std::vector<SessionId> m_members;
    PendingDisconnectTracker m_pendingDisconnectTracker;
    SnapshotBroadcaster m_snapshotBroadcaster; // needs m_members above - declared after it
    GameEndCoordinator m_gameEndCoordinator;   // needs m_session/m_playerAssignment/m_members above - declared last
};
