#include "pch.h"
#include "Pawn.h"
#include "Board.h"
#include <cmath>

Pawn::Pawn(PieceColor color)
    : Piece(color)
{
}

PieceType Pawn::getType() const
{
    return PieceType::Pawn;
}

bool Pawn::isValidMove(
	Position from,
	Position to,
    const Board& board) const
{
    int dr = to.getRow() - from.getRow();
    int dc = to.getCol() - from.getCol();

    // -------------------------
    // White pawn
    // -------------------------
    if (getColor() == PieceColor::White)
    {
        // Move forward
        if (dc == 0 &&
            dr == -1 &&
            board.isEmpty(to))
        {
            return true;
        }

        // Capture diagonally
        if (abs(dc) == 1 &&
            dr == -1 &&
            !board.isEmpty(to) &&
            board.getPieceColor(to) == PieceColor::Black)
        {
            return true;
        }
    }

    // -------------------------
    // Black pawn
    // -------------------------
    else
    {
        // Move forward
        if (dc == 0 &&
            dr == 1 &&
            board.isEmpty(to))
        {
            return true;
        }

        // Capture diagonally
        if (abs(dc) == 1 &&
            dr == 1 &&
            !board.isEmpty(to) &&
            board.getPieceColor(to) == PieceColor::White)
        {
            return true;
        }
    }
    return false;
}