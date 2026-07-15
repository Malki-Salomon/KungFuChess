#include "pch.h"
#include "Bishop.h"
#include <cmath>

Bishop::Bishop(PieceColor color, Position place, PieceType type)
    : Piece(color, place, type)
{
}

PieceType Bishop::getType() const
{
    return PieceType::Bishop;
}

std::string Bishop::getName() const
{
	return this->getColor() == PieceColor::White ? "wB" : "bB";
}

bool Bishop::isValidMove(
    Position from,
    Position to,
    const Board& board) const
{
    int dr = abs(to.getRow() - from.getRow());
    int dc = abs(to.getCol() - from.getCol());

    return dr == dc && dr > 0;
}