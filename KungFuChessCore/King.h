#pragma once

#include "Piece.h"
#include "PieceTypes.h"

class King : public Piece
{
public:
    King(PieceColor color);

    PieceType getType() const override;

    bool isValidMove(
        Position from,
        Position to,
        const Board& board) const override;
};

