#pragma once

#include "SessionManager.h"
#include "PlayerAssignment.h"
#include "../Network/WebSocketServer.h"
#include "../Network/CommandInbox.h"
#include "../Protocol/SnapshotBroadcaster.h"

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

    bool m_running;

    // TODO (Protocol layer): per-connection session routing once multiple
    // concurrent games exist (right now every client shares the single
    // primary session - PlayerAssignment only decides *whose pieces* they
    // may move within that one game, not which game they're in).
};
