#pragma once

#include "Piece.h"

class Rook : public Piece
{
public:
    Rook(char color);

    char getType() const override;

    bool isValidMove(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol,
        const Board& board) const override;
};

