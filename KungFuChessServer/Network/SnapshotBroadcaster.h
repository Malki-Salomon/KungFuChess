#pragma once

#include "IBoardPrinter.h"

class WebSocketServer;

// Attached to a GameSession's controller as its IBoardPrinter (see
// GameSession::attachPrinter). Every time the board changes, Convert() is
// called with the new snapshot; this turns it into a simple text grid and
// broadcasts it to every connected client.
//
// Text format only, matching what TextualBoardPrinting already prints to
// the console - good enough for a first client to parse while the real
// wire protocol still doesn't exist. Expect this class to be replaced by a
// proper JSON serializer once the Protocol layer is designed.
class SnapshotBroadcaster : public IBoardPrinter
{
public:
    explicit SnapshotBroadcaster(WebSocketServer& webSocketServer);

    void Convert(const GameSnapshot& snapshot) override;

private:
    WebSocketServer& m_webSocketServer;
};
