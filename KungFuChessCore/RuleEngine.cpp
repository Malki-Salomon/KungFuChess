#include "pch.h"
#include "RuleEngine.h"
#include "Board.h"
#include "King.h"
#include "Queen.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"
#include "Pawn.h"

bool RuleEngine::isPathClear(Board &board, Position from, Position to)
{
    int dr = 0;
    int dc = 0;

    if (to.getRow() > from.getRow())
        dr = 1;
    else if (to.getRow() < from.getRow())
        dr = -1;

    if (to.getCol() > from.getCol())
        dc = 1;
    else if (to.getCol() < from.getCol())
        dc = -1;

    int r = from.getRow() + dr;
    int c = from.getCol() + dc;

    while (r != to.getRow() || c != to.getCol())
    {
        if (!board.isEmpty(Position(r, c)))
            return false;

        r += dr;
        c += dc;
    }

    return true;
}

bool RuleEngine::isLegalMove(Board &board, Position from, Position to)
{
    if (!board.isInside(from) || !board.isInside(to))
    {
        return false;
    }

    if (board.isEmpty(from))
        return false;

    PieceType selectedType =
        board.getPieceType(from);

    PieceColor selectedColor =
        board.getPieceColor(from);

    if (!board.isEmpty(to) &&
        board.getPieceColor(from) ==
        board.getPieceColor(to))
    {
        return false;
    }

    std::unique_ptr<Piece> piece;

    switch (selectedType)
    {
    case PieceType::King:
        piece = std::make_unique<King>(selectedColor);
        break;

    case PieceType::Queen:
        piece = std::make_unique<Queen>(selectedColor);
        break;

    case PieceType::Rook:
        piece = std::make_unique<Rook>(selectedColor);
        break;

    case PieceType::Bishop:
        piece = std::make_unique<Bishop>(selectedColor);
        break;

    case PieceType::Knight:
        piece = std::make_unique<Knight>(selectedColor);
        break;

    case PieceType::Pawn:
        piece = std::make_unique<Pawn>(selectedColor);
        break;
    }

    if ((selectedType == PieceType::Rook || selectedType == PieceType::Bishop)
        && !isPathClear(board, from, to))
        return false;

    return piece->isValidMove(
		from,
		to,
        board);
}

