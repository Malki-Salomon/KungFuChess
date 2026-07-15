#include "pch.h"
#include "Knight.h"
#include <cmath>

Knight::Knight(PieceColor color, Position place, PieceType type)
    : Piece(color, place, type)
{
}

PieceType Knight::getType() const
{
    return PieceType::Knight;
}

std::string Knight::getName() const
{
    return this->getColor() == PieceColor::White ? "wN" : "bN";
}

bool Knight::isValidMove(
    Position from,
    Position to,
    const Board& board) const
{
    int dr = abs(to.getRow() - from.getRow());
    int dc = abs(to.getCol() - from.getCol());

    return (dr == 2 && dc == 1) ||
        (dr == 1 && dc == 2);
}