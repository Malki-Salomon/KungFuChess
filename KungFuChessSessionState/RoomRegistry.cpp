#include "RoomRegistry.h"

std::string RoomRegistry::createRoomId()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::to_string(m_nextRoomNumber++);
}

std::string RoomRegistry::roomIdForSession(SessionId id) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_sessionRooms.find(id);
    return it == m_sessionRooms.end() ? std::string() : it->second;
}

void RoomRegistry::setSessionRoom(SessionId id, const std::string& roomId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sessionRooms[id] = roomId;
}

void RoomRegistry::clearSessionRoom(SessionId id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sessionRooms.erase(id);
}
