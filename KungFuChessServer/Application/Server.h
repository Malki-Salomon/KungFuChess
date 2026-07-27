#pragma once

#include "SessionManager.h"
#include "../Network/WebSocketServer.h"
#include "../Network/CommandInbox.h"
#include "../Network/SnapshotBroadcaster.h"

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

    bool m_running;

    // TODO (Network/Protocol layer): per-connection session routing (right
    // now every client shares the single primary session), and a real
    // wire protocol instead of plain text passthrough.
};
