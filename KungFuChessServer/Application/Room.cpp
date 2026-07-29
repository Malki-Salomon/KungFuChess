#include "Room.h"

#include "GameSession.h"

#include <algorithm>

Room::Room(std::string roomId, std::unique_ptr<GameSession> session,
           PlayerDirectory& playerDirectory, MatchResultService& matchResultService,
           WebSocketServer& webSocketServer)
    : m_id(std::move(roomId))
    , m_session(std::move(session))
    , m_snapshotBroadcaster(webSocketServer, m_members)
    , m_gameEndCoordinator(*m_session, m_playerAssignment, playerDirectory, matchResultService, webSocketServer, m_members)
{
    m_session->attachPrinter(&m_snapshotBroadcaster);
}

Room::~Room() = default;

const std::string& Room::id() const
{
    return m_id;
}

GameSession& Room::session()
{
    return *m_session;
}

PlayerAssignment& Room::playerAssignment()
{
    return m_playerAssignment;
}

GameEndCoordinator& Room::gameEndCoordinator()
{
    return m_gameEndCoordinator;
}

void Room::addMember(SessionId id)
{
    if (std::find(m_members.begin(), m_members.end(), id) == m_members.end())
        m_members.push_back(id);
}

void Room::removeMember(SessionId id)
{
    m_members.erase(std::remove(m_members.begin(), m_members.end(), id), m_members.end());
}

const std::vector<SessionId>& Room::members() const
{
    return m_members;
}
