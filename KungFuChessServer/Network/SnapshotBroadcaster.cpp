#include "SnapshotBroadcaster.h"

#include "WebSocketServer.h"

#include "GameSnapshot.h"
#include "PieceRegistry.h"

#include <sstream>

SnapshotBroadcaster::SnapshotBroadcaster(WebSocketServer& webSocketServer)
    : m_webSocketServer(webSocketServer)
{
}

void SnapshotBroadcaster::Convert(const GameSnapshot& snapshot)
{
    std::ostringstream out;

    for (const auto& row : snapshot.cells)
    {
        for (const auto& cell : row)
        {
            if (cell.type != PieceType::Empty)
            {
                const auto& meta = PieceRegistry::getMetadata(cell.type);
                out << (cell.color == PieceColor::White ? meta.whiteSymbol : meta.blackSymbol);
            }
            else
            {
                out << '.';
            }
            out << ' ';
        }
        out << '\n';
    }

    m_webSocketServer.broadcast(out.str());
}
