#include "pch.h"
#include "King.h"
#include <cmath>

King::King(char color)
    : Piece(color)
{
}

char King::getType() const
{
    return 'K';
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