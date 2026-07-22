#include "pch.h"
#include "Pawn.h"
#include "Board.h"
#include <cmath>

Pawn::Pawn(PieceColor color, Position place, PieceType type)
    : Piece(color, place, type)
{
}

PieceType Pawn::getType() const
{
    return PieceType::Pawn;
}

std::string Pawn::getName() const
{
    return this->getColor() == PieceColor::White ? "wP" : "bP";
}

bool Pawn::isValidMove(Position from, Position to, const Board& board) const {
    int dr = to.getRow() - from.getRow();
    int dc = to.getCol() - from.getCol();
    int direction = (board.getPieceColor(from) == PieceColor::White) ? -1 : 1;
    int startRow = (board.getPieceColor(from) == PieceColor::White) ? board.getRows() - 2 : 1;

    if (dc == 0 && dr == direction && board.isEmpty(to)) return true;

    if (dc == 0 && dr == 2 * direction && from.getRow() == startRow && board.isEmpty(to)) return true;

    if (abs(dc) == 1 && dr == direction && !board.isEmpty(to) && board.getPieceColor(to) != getColor()) return true;

    return false;
}