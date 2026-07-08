#include "pch.h"
#include "Bishop.h"
#include <cmath>

Bishop::Bishop(char color)
    : Piece(color)
{
}

char Bishop::getType() const
{
    return 'B';
}

bool Bishop::isValidMove(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol,
    const Board&) const
{
    int dr = abs(toRow - fromRow);
    int dc = abs(toCol - fromCol);

    return dr == dc && dr > 0;
}