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
    Position from,
    Position to,
    const Board& board) const
{
    int dr = abs(to.getRow() - from.getRow());
    int dc = abs(to.getCol() - from.getCol());

    return dr == dc && dr > 0;
}