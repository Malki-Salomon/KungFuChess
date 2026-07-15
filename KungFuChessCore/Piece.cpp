#include "pch.h"
#include "Piece.h"


int Piece::nextId = 0;

Piece::Piece(PieceColor color, Position place, PieceType type)
	: color(color), cell(place), type(type), status(PieceStatus::idle), id(nextId++)
{
}

PieceColor Piece::getColor() const
{
    return color;
}

PieceStatus Piece::getStatus() const
{
	return status;
}

void Piece::setStatus(PieceStatus newStatus)
{
	status = newStatus;
}
