#pragma once
#include "Piece.h"
#include "PieceTypes.h"

class Bishop : public Piece
{
public:
    Bishop(PieceColor color);

    PieceType getType() const override;

    bool isValidMove(
        Position from,
        Position to,
        const Board& board) const override;
};

