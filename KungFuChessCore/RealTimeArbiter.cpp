#include "pch.h"
#include "RealTimeArbiter.h"
#include "Board.h"
#include "RuleEngine.h"
#include "Game.h"

void RealTimeArbiter::tick(long long ms, Board& board, RuleEngine& rules, Game& game) {
    for (auto it = activeActions.begin(); it != activeActions.end(); ) {
        it->remainingTime -= ms;

        if (it->remainingTime <= 0) {
            // סיום המהלך - כאן נכנסת הלוגיקה המקורית של ה-Wait
            if (board.getPiece(it->from)->getStatus() != PieceStatus::airborne){
                if (board.getPiece(it->to) != nullptr
                    && board.getPiece(it->to)->getStatus() != PieceStatus::airborne) {
					game.gameOver(board, it->to);
                    board.movePiece(it->from, it->to);
                    // בדיקת קידום כלי (Promotion) וחוקי ניצחון
                    if (rules.canPromote(board, it->to)) {
                        board.promotePiece(it->to, PieceType::Queen);
                    }
					board.removePiece(it->from);
					board.getPiece(it->to)->setStatus(PieceStatus::idle);
                }
                else if (board.getPiece(it->to) != nullptr
                    &&board.getPiece(it->to)->getStatus() == PieceStatus::airborne)
                {
					game.gameOver(board, it->from);
					board.removePiece(it->from);
                }
                else
                {
                    board.movePiece(it->from, it->to);
                    board.removePiece(it->from);
                }
            }
            else {
                // סיום קפיצה
                if (auto p = board.getPiece(it->from)) {
                    p->setStatus(PieceStatus::idle);
                }
            }
            it = activeActions.erase(it); // מחיקת הפעולה מהתור
        }
        else {
            ++it;
        }
    }
}

void RealTimeArbiter::addAction(Position from, Position to, long long duration) {
    ActiveAction newAction;
    newAction.from = from;
    newAction.to = to;
    newAction.remainingTime = duration;

    activeActions.push_back(newAction);
}