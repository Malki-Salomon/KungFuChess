#pragma once

#include "IBoardPrinter.h"
#include "../Network/SessionId.h"

#include <string>
#include <vector>

class WebSocketServer;

// Attached to one room's GameSession as its IBoardPrinter (see
// GameSession::attachPrinter). Every time that room's board changes,
// Convert() is called with the new snapshot; this turns it into JSON and
// sends it to that room's members only (via WebSocketServer::sendToMany())
// - never every connected client server-wide, now that multiple concurrent
// rooms exist. It does not cache anything or answer questions about board
// state. A caller that needs to know what a room's board currently looks
// like (e.g. to check move ownership, or to catch a newly-joined
// connection up) should query GameSession::getSnapshot() instead, which
// reads straight from Core rather than from a second, potentially-stale
// copy here.
class SnapshotBroadcaster : public IBoardPrinter
{
public:
    // members is a reference to the owning Room's own member list (see
    // Room) - kept live for as long as this SnapshotBroadcaster is, same
    // lifetime precedent as GameEndCoordinator's equivalent reference.
    SnapshotBroadcaster(WebSocketServer& webSocketServer, const std::vector<SessionId>& members);

    void Convert(const GameSnapshot& snapshot) override;

    // Turns a Core snapshot into the exact BoardMessage wire text Convert()
    // itself sends - exposed statically so a caller that needs an
    // on-demand board message (e.g. Server's join-room catch-up, sending
    // one connection the room's current state immediately rather than
    // waiting for the next change) can reuse the same conversion instead
    // of duplicating it.
    static std::string buildBoardMessage(const GameSnapshot& snapshot);

private:
    WebSocketServer& m_webSocketServer;
    const std::vector<SessionId>& m_members;
};
