#include "pch.h"
#include "Piece.h"

Piece::Piece(char color)
    : color(color)
{
}

char Piece::getColor() const
{
    return color;
}