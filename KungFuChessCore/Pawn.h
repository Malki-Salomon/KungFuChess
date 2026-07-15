#pragma once

#include "Piece.h"
#include "PieceTypes.h"

class Pawn : public Piece
{
public:
    Pawn(PieceColor color, Position place, PieceType type);

    PieceType getType() const override;

    std::string getName() const override;

    bool isValidMove(
        Position from,
        Position to,
        const Board& board) const override;
};

