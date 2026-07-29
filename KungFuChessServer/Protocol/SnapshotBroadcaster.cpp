#include "SnapshotBroadcaster.h"

#include "../Network/WebSocketServer.h"
#include "BoardMessage.h"

#include "GameSnapshot.h"
#include "PieceRegistry.h"

SnapshotBroadcaster::SnapshotBroadcaster(WebSocketServer& webSocketServer, const std::vector<SessionId>& members)
    : m_webSocketServer(webSocketServer)
    , m_members(members)
{
}

void SnapshotBroadcaster::Convert(const GameSnapshot& snapshot)
{
    m_webSocketServer.sendToMany(m_members, buildBoardMessage(snapshot));
}

std::string SnapshotBroadcaster::buildBoardMessage(const GameSnapshot& snapshot)
{
    // This function's only job is turning Core's snapshot into the plain
    // symbol-string grid BoardMessage's shape expects - the actual JSON
    // shape/serialization is BoardMessage's job (shared with the client's
    // parse side), not this one's.
    std::vector<std::vector<std::string>> cells(snapshot.cells.size());

    for (size_t row = 0; row < snapshot.cells.size(); ++row)
    {
        const auto& rowCells = snapshot.cells[row];
        cells[row].resize(rowCells.size());

        for (size_t col = 0; col < rowCells.size(); ++col)
        {
            const auto& cell = rowCells[col];
            if (cell.type != PieceType::Empty)
            {
                const auto& meta = PieceRegistry::getMetadata(cell.type);
                cells[row][col] = (cell.color == PieceColor::White) ? meta.whiteSymbol : meta.blackSymbol;
            }
            else
            {
                cells[row][col] = ".";
            }
        }
    }

    return BoardMessage::build(cells);
}
