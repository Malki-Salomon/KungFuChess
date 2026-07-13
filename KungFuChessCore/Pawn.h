#pragma once

#include "Piece.h"
#include "PieceTypes.h"

class Pawn : public Piece
{
public:
    Pawn(PieceColor color);

    PieceType getType() const override;

    bool isValidMove(
        Position from,
        Position to,
        const Board& board) const override;
};

