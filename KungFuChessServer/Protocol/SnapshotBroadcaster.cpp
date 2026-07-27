#include "SnapshotBroadcaster.h"

#include "../Network/WebSocketServer.h"

//#include "GameSnapshot.h"
#include "PieceRegistry.h"

#include <sstream>

SnapshotBroadcaster::SnapshotBroadcaster(WebSocketServer& webSocketServer)
    : m_webSocketServer(webSocketServer)
{
}

void SnapshotBroadcaster::Convert(const GameSnapshot& snapshot)
{
    // {"type":"board","cells":[["bR","bN",...],["bP",...],...]}
    // Piece symbols (PieceRegistry's whiteSymbol/blackSymbol, e.g. "wP",
    // "bR") are plain two-character strings with no quotes/backslashes, so
    // no escaping is needed here.
    std::ostringstream out;
    out << "{\"type\":\"board\",\"cells\":[";

    for (size_t row = 0; row < snapshot.cells.size(); ++row)
    {
        if (row > 0)
            out << ",";
        out << "[";

        const auto& rowCells = snapshot.cells[row];
        for (size_t col = 0; col < rowCells.size(); ++col)
        {
            if (col > 0)
                out << ",";

            const auto& cell = rowCells[col];
            if (cell.type != PieceType::Empty)
            {
                const auto& meta = PieceRegistry::getMetadata(cell.type);
                const std::string& symbol = (cell.color == PieceColor::White) ? meta.whiteSymbol : meta.blackSymbol;
                out << "\"" << symbol << "\"";
            }
            else
            {
                out << "\".\"";
            }
        }

        out << "]";
    }

    out << "]}";
    m_webSocketServer.broadcast(out.str());
}
