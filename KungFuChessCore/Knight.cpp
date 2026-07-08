#include "pch.h"
#include "Knight.h"
#include <cmath>

Knight::Knight(char color)
    : Piece(color)
{
}

char Knight::getType() const
{
    return 'N';
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