#pragma once
#include "Board.h"
#include "Piece.h"
#include "PieceTypes.h"

class Queen : public Piece
{
public:
    Queen(PieceColor color);

    PieceType getType() const override;

    bool isValidMove(
        Position from,
        Position to,
        const Board& board) const override;
};

