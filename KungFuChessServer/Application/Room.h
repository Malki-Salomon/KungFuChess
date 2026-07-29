#pragma once

#include "PlayerAssignment.h"
#include "GameEndCoordinator.h"
#include "../Protocol/SnapshotBroadcaster.h"
#include "../Network/SessionId.h"

#include <memory>
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

private:
    std::string m_id;
    std::unique_ptr<GameSession> m_session;
    PlayerAssignment m_playerAssignment;
    std::vector<SessionId> m_members;
    SnapshotBroadcaster m_snapshotBroadcaster; // needs m_members above - declared after it
    GameEndCoordinator m_gameEndCoordinator;   // needs m_session/m_playerAssignment/m_members above - declared last
};
