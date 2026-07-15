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

    if(board.getPiece(from)->getStatus() == PieceStatus::airborne)
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

    /*std::unique_ptr<Piece> piece;

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
    }*/

    if ((selectedType == PieceType::Rook || selectedType == PieceType::Bishop
        || selectedType == PieceType::Pawn)
        && !isPathClear(board, from, to))
        return false;

    return board.getPiece(from)->isValidMove(from, to, board);
    /*return piece->isValidMove(
		from,
		to,
        board);*/
}

//GameStatus RuleEngine::evaluate(const Board& board)
//{
//    bool whiteKingExists = false;
//    bool blackKingExists = false;
//
//    for (int r = 0; r < board.getRows(); ++r) {
//        for (int c = 0; c < board.getCols(); ++c) {
//            if (!board.isEmpty(Position(r, c))) {
//                PieceType type = board.getPieceType(Position(r, c));
//
//                if (type == PieceType::King) {
//                    if (board.getPieceColor(Position(r, c)) == PieceColor::White)
//                        whiteKingExists = true;
//                    else
//                        blackKingExists = true;
//                }
//            }
//        }
//    }
//
//    if (!whiteKingExists) return GameStatus::BlackWins;
//    if (!blackKingExists) return GameStatus::WhiteWins;
//
//    return GameStatus::Playing;
//}

bool RuleEngine::canPromote(const Board& board, Position pos) {
    Piece* p = board.getPiece(pos);
    if (!p || p->getType() != PieceType::Pawn) return false;

    int lastRow = (p->getColor() == PieceColor::White) ? 0 : board.getRows() - 1;
    return pos.getRow() == lastRow;
}

