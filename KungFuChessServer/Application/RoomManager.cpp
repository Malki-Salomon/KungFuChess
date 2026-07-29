#include "RoomManager.h"

#include "Room.h"
#include "GameFactory.h"
#include "GameSession.h"
#include "IGameController.h"

RoomManager::RoomManager(PlayerDirectory& playerDirectory, MatchResultService& matchResultService,
                         WebSocketServer& webSocketServer)
    : m_playerDirectory(playerDirectory)
    , m_matchResultService(matchResultService)
    , m_webSocketServer(webSocketServer)
{
}

RoomManager::~RoomManager() = default;

std::string RoomManager::createRoom()
{
    std::string roomId = m_registry.createRoomId();
    auto session = std::make_unique<GameSession>(GameFactory::createStandardGame());
    m_rooms.push_back(std::make_unique<Room>(roomId, std::move(session), m_playerDirectory, m_matchResultService, m_webSocketServer));
    return roomId;
}

Room* RoomManager::findRoom(const std::string& roomId)
{
    for (auto& room : m_rooms)
    {
        if (room->id() == roomId)
            return room.get();
    }
    return nullptr;
}

Room* RoomManager::roomForSession(SessionId id)
{
    std::string roomId = m_registry.roomIdForSession(id);
    if (roomId.empty())
        return nullptr;
    return findRoom(roomId);
}

void RoomManager::setSessionRoom(SessionId id, const std::string& roomId)
{
    m_registry.setSessionRoom(id, roomId);
}

void RoomManager::clearSessionRoom(SessionId id)
{
    m_registry.clearSessionRoom(id);
}

const std::vector<std::unique_ptr<Room>>& RoomManager::allRooms() const
{
    return m_rooms;
}
