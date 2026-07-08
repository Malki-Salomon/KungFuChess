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
    int fromRow,
    int fromCol,
    int toRow,
    int toCol,
    const Board& board) const
{
    int dr = toRow - fromRow;
    int dc = toCol - fromCol;

    // -------------------------
    // White pawn
    // -------------------------
    if (getColor() == PieceColor::White)
    {
        // Move forward
        if (dc == 0 &&
            dr == -1 &&
            board.isEmpty(toRow, toCol))
        {
            return true;
        }

        // Capture diagonally
        if (abs(dc) == 1 &&
            dr == -1 &&
            !board.isEmpty(toRow, toCol) &&
            board.getPieceColor(toRow, toCol) == PieceColor::Black)
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
            board.isEmpty(toRow, toCol))
        {
            return true;
        }

        // Capture diagonally
        if (abs(dc) == 1 &&
            dr == 1 &&
            !board.isEmpty(toRow, toCol) &&
            board.getPieceColor(toRow, toCol) == PieceColor::White)
        {
            return true;
        }
    }

    return false;
}