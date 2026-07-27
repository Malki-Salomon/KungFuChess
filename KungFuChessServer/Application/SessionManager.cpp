#include "SessionManager.h"

#include "GameFactory.h"
#include "GameSession.h"
#include "IGameController.h"

SessionManager::SessionManager()
{
    // Temporary: auto-create one session so the server has something to run
    // before the Network layer exists. Once client connections drive session
    // creation, this call will move out of the constructor.
    createSession();
}

SessionManager::~SessionManager() = default;

void SessionManager::createSession()
{
    m_sessions.push_back(std::make_unique<GameSession>(GameFactory::createStandardGame()));
}

void SessionManager::destroyAllSessions()
{
    m_sessions.clear();
}

void SessionManager::tickAllSessions()
{
    for (auto& session : m_sessions)
    {
        session->tick();
    }
}

GameSession* SessionManager::getPrimarySession()
{
    return m_sessions.empty() ? nullptr : m_sessions.front().get();
}
