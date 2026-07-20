#include "pch.h"
#include "RuleEngine.h"
#include "Board.h"

bool RuleEngine::isLegalMove(Board& board, Position from, Position to)
{
    if (!board.isInside(from) || !board.isInside(to))
    {
        return false;
    }

    if (board.isEmpty(from))
        return false;

    if (board.getPiece(from)->getStatus() == PieceStatus::airborne)
        return false;

    PieceType selectedType = board.getPieceType(from);

    if (!board.isEmpty(to) &&
        board.getPieceColor(from) == board.getPieceColor(to))
    {
        return false;
    }

    if ((selectedType == PieceType::Rook || selectedType == PieceType::Bishop
        || selectedType == PieceType::Pawn)
        && !isPathClear(board, from, to))
        return false;

    return board.getPiece(from)->isValidMove(from, to, board);
}

bool RuleEngine::isPathClear(Board& board, Position from, Position to)
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

bool RuleEngine::canPromote(const Board& board, Position pos)
{
    Piece* p = board.getPiece(pos);
    if (!p || p->getType() != PieceType::Pawn)
        return false;

    int lastRow = (p->getColor() == PieceColor::White) ? 0 : board.getRows() - 1;
    return pos.getRow() == lastRow;
}
