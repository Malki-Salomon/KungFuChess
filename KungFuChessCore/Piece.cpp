#include "pch.h"
#include "Piece.h"

Piece::Piece(PieceColor color)
    : color(color)
{
}

PieceColor Piece::getColor() const
{
    return color;
}