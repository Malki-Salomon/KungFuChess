#pragma once

#include "Piece.h"
#include "PieceTypes.h"

class King : public Piece
{
public:
    King(PieceColor color);

    PieceType getType() const override;

    bool isValidMove(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol,
        const Board& board) const override;
};

