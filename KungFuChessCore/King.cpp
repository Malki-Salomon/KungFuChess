#include "pch.h"
#include "King.h"
#include <cmath>

King::King(PieceColor color, Position place, PieceType type)
    : Piece(color, place, type)
{
}

PieceType King::getType() const
{
    return PieceType::King;
}

std::string King::getName() const
{
    return this->getColor() == PieceColor::White ? "wK" : "bK";
}

bool King::isValidMove(
    Position from,
    Position to,
    const Board& board) const
{
    int dr = abs(to.getRow() - from.getRow());
    int dc = abs(to.getCol() - from.getCol());

    return dr <= 1 && dc <= 1 && (dr != 0 || dc != 0);
}