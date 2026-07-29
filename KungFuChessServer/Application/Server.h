#pragma once

#include "RoomManager.h"
#include "PlayerDirectory.h"
#include "../Network/WebSocketServer.h"
#include "../Network/CommandInbox.h"
#include "SqliteUserRepository.h"
#include "AuthService.h"
#include "AuthWorker.h"
#include "MatchResultService.h"

class Server
{
public:
    Server();

    void run();

private:
    // Removes a connection's membership/seat from whatever room it
    // currently belongs to (if any) and clears the session->room mapping.
    // Shared by disconnect cleanup and by joinRoom handling (a connection
    // switching rooms must not leave stale membership behind in the one
    // it's leaving) - both are "minimal hygiene," not the reconnection/
    // countdown feature, which is explicitly out of scope for this stage.
    void leaveCurrentRoom(SessionId id);


    WebSocketServer m_webSocketServer;
    CommandInbox m_commandInbox;
    PlayerDirectory m_playerDirectory;
    SqliteUserRepository m_userRepository; // real, file-backed persistence (stage 4b) - see KungFuChessAccounts/README.md
    AuthService m_authService;
    AuthWorker m_authWorker; // runs register/login off the network I/O thread (stage 4d) - depends on m_webSocketServer/m_authService/m_playerDirectory above, so must stay declared after all three
    MatchResultService m_matchResultService; // depends on m_userRepository above
    RoomManager m_roomManager; // owns every active Room (each with its own GameSession/PlayerAssignment/GameEndCoordinator) - depends on m_playerDirectory, m_matchResultService, m_webSocketServer, all declared above

    bool m_running;
};
