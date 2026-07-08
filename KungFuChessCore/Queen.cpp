#include "pch.h"
#include "Queen.h"
#include <cmath>

Queen::Queen(char color)
    : Piece(color)
{
}

char Queen::getType() const
{
    return 'Q';
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