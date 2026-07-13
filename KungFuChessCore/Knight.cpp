#include "pch.h"
#include "Knight.h"
#include <cmath>

Knight::Knight(PieceColor color)
    : Piece(color)
{
}

PieceType Knight::getType() const
{
    return PieceType::Knight;
}

bool Knight::isValidMove(
    Position from,
    Position to,
    const Board& board) const
{
    int dr = abs(to.getRow() - from.getRow());
    int dc = abs(to.getCol() - from.getCol());

    return (dr == 2 && dc == 1) ||
        (dr == 1 && dc == 2);
}