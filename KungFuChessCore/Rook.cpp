#include "pch.h"
#include "Rook.h"
#include <cmath>

Rook::Rook(PieceColor color)
    : Piece(color)
{
}

PieceType Rook::getType() const
{
    return PieceType::Rook;
}

bool Rook::isValidMove(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol,
    const Board& board) const
{
    return ((fromRow != toRow && fromCol == toCol) || (fromRow == toRow && fromCol != toCol))
            && board.isPathClear(fromRow, fromCol, toRow, toCol);
}