#include "pch.h"
#include "Controller.h"
#include "RuleEngine.h"
#include "Game.h"

void Controller::executeClick(int col, int row, Game& game, RealTimeArbiter& arbiter)
{
    if (!game.isGameActive())
        return;

    Position place(row, col);

    if (!game.getBoard().isInside(place))
        return;

    if (!game.gethasSelection())
    {
        if (!game.getBoard().isEmpty(place) &&
            game.getBoard().getPiece(place)->getStatus() == PieceStatus::idle)
        {
            game.setHasSelection(true);
            game.setSelectedPiece(place);
        }
        return;
    }

    if (!game.getBoard().isEmpty(place) &&
        game.getBoard().getPieceColor(place) ==
        game.getBoard().getPieceColor(Position(game.getSelectedPiece().getRow(), game.getSelectedPiece().getCol())))
    {
        game.setSelectedPiece(place);
        return;
    }

    if (!RuleEngine::isLegalMove(const_cast<Board&>(game.getBoard()), game.getSelectedPiece(), place))
    {
        return;
    }

    long long remainingMoveTime =
        std::max(abs(place.getRow() - game.getSelectedPiece().getRow()),
            abs(place.getCol() - game.getSelectedPiece().getCol())) * 1000;

    arbiter.addAction(game.getSelectedPiece(), place, remainingMoveTime);

    game.getBoard().getPiece(game.getSelectedPiece())->setStatus(PieceStatus::moving);

    game.setHasSelection(false);
}

void Controller::executeJump(int col, int row, Game& game, RealTimeArbiter& arbiter)
{
    Position place(row, col);
    Piece* p = game.getBoard().getPiece(place);

    if (p && p->getStatus() == PieceStatus::idle) {
        arbiter.addAction(place, place, 1000);
        p->setStatus(PieceStatus::airborne);
    }
}