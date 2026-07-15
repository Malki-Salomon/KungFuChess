#pragma once

#include "Piece.h"
#include "PieceTypes.h"

class King : public Piece
{
public:
    King(PieceColor color, Position place, PieceType type);

    PieceType getType() const override;

    std::string getName() const override;

    bool isValidMove(
        Position from,
        Position to,
        const Board& board) const override;
};

