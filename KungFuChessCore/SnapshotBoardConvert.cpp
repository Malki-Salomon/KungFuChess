#include "pch.h"
#include "SnapshotBoardConvert.h"
#include "Board.h"
#include "PieceFactory.h"

SnapshotBoardConvert::SnapshotBoardConvert(const GameSnapshot& snapshot)
    : snapshot(snapshot)
{
}

bool SnapshotBoardConvert::Convert(Board& board)
{
    board.clear();

    const auto& cells = snapshot.cells;

    if (cells.empty())
        return true;

    size_t numCols = cells[0].size();

    for (const auto& row : cells)
    {
        if (row.size() != numCols)
        {
            board.setError("ERROR ROW_WIDTH_MISMATCH");
            return false;
        }
    }

    board.setSize(static_cast<int>(cells.size()), static_cast<int>(numCols));

    for (int row = 0; row < static_cast<int>(cells.size()); ++row)
    {
        for (int col = 0; col < static_cast<int>(numCols); ++col)
        {
            Position pos(row, col);
            const PieceSnapshot& piece = cells[row][col];
            board.setPiece(pos, PieceFactory::create(piece.type, piece.color, pos));
        }
    }

    return true;
}
