#include "pch.h"
#include "Controller.h"
#include "RuleEngine.h"
#include "BoardMapper.h"
#include "Game.h"

void Controller::executeClick(int x, int y, Game& game, RealTimeArbiter& arbiter)
{
    if (!game.isGameActive())
        return;

    Position place = BoardMapper::pixelToCell(y, x);

    if (!game.getBoard().isInside(place))
        return;

    // אין כלי מסומן - בוחרים כלי
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

void Controller::executeJump(int x, int y, Game& game, RealTimeArbiter& arbiter)
{
    Position place = BoardMapper::pixelToCell(y, x);
    Piece* p = game.getBoard().getPiece(place);

    if (p && p->getStatus() == PieceStatus::idle) {
        arbiter.addAction(game.getSelectedPiece(), place, 1000);
        p->setStatus(PieceStatus::airborne);
    }
}