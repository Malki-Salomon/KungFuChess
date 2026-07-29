#pragma once

#include "SessionManager.h"
#include "PlayerAssignment.h"
#include "PlayerDirectory.h"
#include "../Network/WebSocketServer.h"
#include "../Network/CommandInbox.h"
#include "../Protocol/SnapshotBroadcaster.h"
#include "SqliteUserRepository.h"
#include "AuthService.h"
#include "AuthWorker.h"
#include "MatchResultService.h"
#include "GameEndCoordinator.h"

class Server
{
public:
    Server();

    void run();

private:
    SessionManager m_sessionManager;
    WebSocketServer m_webSocketServer;
    CommandInbox m_commandInbox;
    SnapshotBroadcaster m_snapshotBroadcaster;
    PlayerAssignment m_playerAssignment;
    PlayerDirectory m_playerDirectory;
    SqliteUserRepository m_userRepository; // real, file-backed persistence (stage 4b) - see KungFuChessAccounts/README.md
    AuthService m_authService;
    AuthWorker m_authWorker; // runs register/login off the network I/O thread (stage 4d) - depends on m_webSocketServer/m_authService/m_playerDirectory above, so must stay declared after all three
    MatchResultService m_matchResultService; // depends on m_userRepository above
    GameEndCoordinator m_gameEndCoordinator; // detects the Playing->finished transition and closes the loop (ratings + broadcast) - depends on m_sessionManager's primary GameSession, m_playerAssignment, m_playerDirectory, m_matchResultService, m_webSocketServer, all declared above

    bool m_running;

    // TODO (Protocol layer): per-connection session routing once multiple
    // concurrent games exist (right now every client shares the single
    // primary session - PlayerAssignment only decides *whose pieces* they
    // may move within that one game, not which game they're in).
};
