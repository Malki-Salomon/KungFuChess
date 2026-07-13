#include "pch.h"
#include "Rook.h"
#include <cmath>

Rook::Rook(PieceColor color)
    : Piece(color)
{
}

PieceType Rook::getType() const
{
    return PieceType::Rook;
}

bool Rook::isValidMove(
	Position from,
	Position to,
    const Board& board) const
{
    return ((from.getRow() != to.getRow() && from.getCol() == to.getCol()) || 
        (from.getRow() == to.getRow() && from.getCol() != to.getCol()));
}