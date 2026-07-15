#pragma once
#include "Board.h"
#include "Piece.h"
#include "PieceTypes.h"

class Rook : public Piece
{
public:
    Rook(PieceColor color, Position place, PieceType type);

    PieceType getType() const override;

    std::string getName() const override;

    bool isValidMove(
        Position from,
        Position to,
        const Board& board) const override;
};

