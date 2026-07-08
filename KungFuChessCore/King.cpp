#include "pch.h"
#include "King.h"
#include <cmath>

King::King(PieceColor color)
    : Piece(color)
{
}

PieceType King::getType() const
{
    return PieceType::King;
}

bool King::isValidMove(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol,
    const Board&) const
{
    int dr = abs(toRow - fromRow);
    int dc = abs(toCol - fromCol);

    return dr <= 1 && dc <= 1 && (dr != 0 || dc != 0);
}