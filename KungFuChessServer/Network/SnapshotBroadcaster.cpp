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
    // Update the ownership cache first (cheap - just copying colors).
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_colorCache.assign(snapshot.cells.size(), {});
        for (size_t row = 0; row < snapshot.cells.size(); ++row)
        {
            m_colorCache[row].resize(snapshot.cells[row].size());
            for (size_t col = 0; col < snapshot.cells[row].size(); ++col)
            {
                m_colorCache[row][col] = snapshot.cells[row][col].color;
            }
        }
    }

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

PieceColor SnapshotBroadcaster::colorAt(int row, int col) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (row < 0 || col < 0 || static_cast<size_t>(row) >= m_colorCache.size())
        return PieceColor::None;
    if (static_cast<size_t>(col) >= m_colorCache[row].size())
        return PieceColor::None;

    return m_colorCache[row][col];
}
