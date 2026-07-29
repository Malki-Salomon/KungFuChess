#pragma once

#include "RoomRegistry.h"
#include "SessionId.h"

#include <memory>
#include <string>
#include <vector>

class Room;
class PlayerDirectory;
class MatchResultService;
class WebSocketServer;

// Owns every currently active Room and delegates the dependency-free
// bookkeeping of which room each connection belongs to to RoomRegistry
// (see its header for why that split exists - Room's own dependency
// graph, unlike RoomRegistry's, can't be unit-tested directly).
// Replaces SessionManager's role now that games are created on demand,
// per room, instead of one fixed game at startup.
class RoomManager
{
public:
    RoomManager(PlayerDirectory& playerDirectory, MatchResultService& matchResultService,
                WebSocketServer& webSocketServer);
    ~RoomManager(); // declared here, defined in the .cpp where Room is complete

    // Creates a fresh GameSession (via GameFactory, exactly as
    // SessionManager used to at startup) inside a brand new Room, and
    // returns its id.
    std::string createRoom();

    // Returns nullptr if the code doesn't exist.
    Room* findRoom(const std::string& roomId);

    // Which room (if any) a connection currently belongs to.
    Room* roomForSession(SessionId id);
    void setSessionRoom(SessionId id, const std::string& roomId);
    void clearSessionRoom(SessionId id);

    // Every currently active room - Server::run() ticks all of them each
    // iteration.
    const std::vector<std::unique_ptr<Room>>& allRooms() const;

private:
    RoomRegistry m_registry;
    PlayerDirectory& m_playerDirectory;
    MatchResultService& m_matchResultService;
    WebSocketServer& m_webSocketServer;
    std::vector<std::unique_ptr<Room>> m_rooms;
};
