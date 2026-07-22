#include "pch.h"
#include "RealTimeArbiter.h"
#include "Board.h"
#include "Game.h"
#include "RuleEngine.h"
#include <iostream>

void RealTimeArbiter::addAction(Position from, Position to, long long duration)
{
    ActiveAction newAction;
    newAction.from = from;
    newAction.to = to;
    newAction.remainingTime = duration;

    activeActions.push_back(newAction);
}

bool RealTimeArbiter::tick(long long ms, Board& board, RuleEngine& rules, Game& game)
{
	bool anyActionCompleted = false;
    for (auto it = activeActions.begin(); it != activeActions.end(); )
    {
        it->remainingTime -= ms;

        if (it->remainingTime <= 0)
        {
            if (board.getPiece(it->from)->getStatus() != PieceStatus::airborne)
            {
                if (board.getPiece(it->to) != nullptr
                    && board.getPiece(it->to)->getStatus() != PieceStatus::airborne)
                {
                    game.gameOver(board, it->to);
                    board.movePiece(it->from, it->to);

                    if (rules.canPromote(board, it->to))
                    {
                        board.promotePiece(it->to, PieceType::Queen);
                    }
                    board.removePiece(it->from);
                    board.getPiece(it->to)->setStatus(PieceStatus::idle);
                }
                else if (board.getPiece(it->to) != nullptr
                    && board.getPiece(it->to)->getStatus() == PieceStatus::airborne)
                {
                    game.gameOver(board, it->from);
                    board.removePiece(it->from);
                }
                else
                {
                    board.movePiece(it->from, it->to);
                    board.removePiece(it->from);
					board.getPiece(it->to)->setStatus(PieceStatus::idle);
                }
            }
            else
            {
                if (auto p = board.getPiece(it->from))
                {
                    p->setStatus(PieceStatus::idle);
                }
            }
            it = activeActions.erase(it); 
			anyActionCompleted = true;
        }
        else
        {
            ++it;
        }
    }
	return anyActionCompleted;
}
