#pragma once

#include "Piece.h"
#include "PieceTypes.h"


class Knight : public Piece
{
public:
    Knight(PieceColor color);

    PieceType getType() const override;

    bool isValidMove(
        Position from,
        Position to,
        const Board& board) const override;
};

