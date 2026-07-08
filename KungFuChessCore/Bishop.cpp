#include "pch.h"
#include "Bishop.h"
#include <cmath>

Bishop::Bishop(PieceColor color)
    : Piece(color)
{
}

PieceType Bishop::getType() const
{
    return PieceType::Bishop;
}

bool Bishop::isValidMove(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol,
    const Board& board) const
{
    int dr = abs(toRow - fromRow);
    int dc = abs(toCol - fromCol);

    return dr == dc && dr > 0 && board.isPathClear(fromRow, fromCol, toRow, toCol);
}