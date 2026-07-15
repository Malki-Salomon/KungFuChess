#pragma once
#include "PieceTypes.h"
#include "Position.h"
#include <string>

class Board;

class Piece
{
protected:
    int id;
    PieceColor color;
	PieceType type;
    Position cell;
    PieceStatus status;
	static int nextId;


public:
    Piece(PieceColor color, Position place, PieceType type);
    virtual ~Piece() = default;

    PieceColor getColor() const;

	PieceStatus getStatus() const;

	void setStatus(PieceStatus newStatus);

    virtual PieceType getType() const = 0;

    virtual std::string getName() const = 0;

    virtual bool isValidMove(
		Position from,
        Position to,
        const Board& board) const = 0;
};

