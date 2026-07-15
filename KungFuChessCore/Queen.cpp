#include "pch.h"
#include "Queen.h"
#include <cmath>

Queen::Queen(PieceColor color, Position place, PieceType type)
    : Piece(color, place, type)
{
}

PieceType Queen::getType() const
{
    return PieceType::Queen;
}

std::string Queen::getName() const
{
    return this->getColor() == PieceColor::White ? "wQ" : "bQ";
}

bool Queen::isValidMove(
	Position from,
	Position to,
    const Board& board) const
{
    int dr = abs(to.getRow() - from.getRow());
    int dc = abs(to.getCol() - from.getCol());

    return ((dr == dc && dr > 0) || (dr == 0 && dc > 0) || (dc == 0 && dr > 0));
}