#include "pch.h"
#include "Rook.h"
#include <cmath>

Rook::Rook(PieceColor color, Position place, PieceType type)
    : Piece(color, place, type)
{
}

PieceType Rook::getType() const
{
    return PieceType::Rook;
}

std::string Rook::getName() const
{
    return this->getColor() == PieceColor::White ? "wR" : "bR";
}

bool Rook::isValidMove(
	Position from,
	Position to,
    const Board& board) const
{
    return ((from.getRow() != to.getRow() && from.getCol() == to.getCol()) || 
        (from.getRow() == to.getRow() && from.getCol() != to.getCol()));
}