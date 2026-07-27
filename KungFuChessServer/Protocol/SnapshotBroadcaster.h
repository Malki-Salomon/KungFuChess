#pragma once

#include "IBoardPrinter.h"

class WebSocketServer;

// Attached to a GameSession's controller as its IBoardPrinter (see
// GameSession::attachPrinter). Every time the board changes, Convert() is
// called with the new snapshot; this turns it into JSON and broadcasts it
// to every connected client. That's its only job - it does not cache
// anything or answer questions about board state. A caller that needs to
// know what the board currently looks like (e.g. to check move ownership)
// should query GameSession::getSnapshot() instead, which reads straight
// from Core rather than from a second, potentially-stale copy here.
class SnapshotBroadcaster : public IBoardPrinter
{
public:
    explicit SnapshotBroadcaster(WebSocketServer& webSocketServer);

    void Convert(const GameSnapshot& snapshot) override;

private:
    WebSocketServer& m_webSocketServer;
};
