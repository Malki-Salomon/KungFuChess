#pragma once

#include <memory>
#include <vector>

class GameSession;

class SessionManager
{
public:
    SessionManager();
    ~SessionManager(); // declared here, defined in the .cpp where GameSession is complete

    void createSession();
    void destroyAllSessions();

    void tickAllSessions();

    // POC only: returns the first session, or nullptr if none exist yet.
    // Once connections map to their own sessions, this goes away in favor
    // of routing each connection to its specific GameSession.
    GameSession* getPrimarySession();

private:
    std::vector<std::unique_ptr<GameSession>> m_sessions;
};
