#include "pch.h"
#include "Rook.h"
#include <cmath>

Rook::Rook(char color)
    : Piece(color)
{
}

char Rook::getType() const
{
    return 'R';
}

bool Rook::isValidMove(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol,
    const Board&) const
{
    return (fromRow != toRow && fromCol == toCol) || (fromRow == toRow && fromCol != toCol);
}