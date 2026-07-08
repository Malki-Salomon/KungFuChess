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
        int fromRow,
        int fromCol,
        int toRow,
        int toCol,
        const Board& board) const override;
};

