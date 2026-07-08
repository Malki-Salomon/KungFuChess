#include "pch.h"
#include "Queen.h"
#include <cmath>

Queen::Queen(PieceColor color)
    : Piece(color)
{
}

PieceType Queen::getType() const
{
    return PieceType::Queen;
}

bool Queen::isValidMove(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol,
    const Board&) const
{
    int dr = abs(toRow - fromRow);
    int dc = abs(toCol - fromCol);

    return (dr == dc && dr > 0) ||
        (dr == 0 && dc > 0) ||
        (dc == 0 && dr > 0);
}