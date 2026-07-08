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
    int fromRow,
    int fromCol,
    int toRow,
    int toCol,
    const Board&) const
{
    int dr = abs(toRow - fromRow);
    int dc = abs(toCol - fromCol);

    return (dr == 2 && dc == 1) ||
        (dr == 1 && dc == 2);
}