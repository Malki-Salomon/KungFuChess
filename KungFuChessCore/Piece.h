#pragma once
#include "PieceTypes.h"
#include "Position.h"

class Board;

class Piece
{
protected:
    PieceColor color;
	PieceType type;


public:
    Piece(PieceColor color);
    virtual ~Piece() = default;

    PieceColor getColor() const;

    virtual PieceType getType() const = 0;

    virtual bool isValidMove(
		Position from,
        Position to,
        const Board& board) const = 0;
};

