#pragma once

#include "IBoardPrinter.h"

#include <mutex>
#include <vector>

class WebSocketServer;

// Attached to a GameSession's controller as its IBoardPrinter (see
// GameSession::attachPrinter). Every time the board changes, Convert() is
// called with the new snapshot; this does two things with it:
//   1. Turns it into JSON and broadcasts it to every connected client.
//   2. Caches which color (if any) owns each square, so Server can check
//      "is this connection allowed to move this piece" before dispatching
//      a move - without needing any new query API on IGameController
//      itself (which has none; it's push-only via this printer).
class SnapshotBroadcaster : public IBoardPrinter
{
public:
    explicit SnapshotBroadcaster(WebSocketServer& webSocketServer);

    void Convert(const GameSnapshot& snapshot) override;

    // Thread-safe. Returns PieceColor::None if out of range or if no piece
    // has ever been seen there (including simply not having received a
    // snapshot yet).
    PieceColor colorAt(int row, int col) const;

private:
    WebSocketServer& m_webSocketServer;

    mutable std::mutex m_mutex;
    std::vector<std::vector<PieceColor>> m_colorCache;
};
