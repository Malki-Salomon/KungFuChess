#pragma once
#include "PieceTypes.h"

class Board;

class Piece
{
protected:
    PieceColor color;

public:
    Piece(PieceColor color);
    virtual ~Piece() = default;

    PieceColor getColor() const;

    virtual PieceType getType() const = 0;

    virtual bool isValidMove(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol,
        const Board& board) const = 0;
};

