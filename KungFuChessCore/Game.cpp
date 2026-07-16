#include "pch.h"
#include "Board.h"

#include "Game.h"
#include "Bishop.h"
#include "Command.h"
#include "King.h"
#include "Knight.h"
#include "Pawn.h"
#include "Queen.h"
#include "Rook.h"
#include "TextBoardConvert.h"
#include "TextualBoardPrinting.h"
#include "IBoardPrinter.h"
#include <iostream>
#include <memory>
#include "RuleEngine.h"
#include "StringCommandConvert.h"
#include "Position.h"
#include "BoardMapper.h"
#include "RealTimeArbiter.h"
#include "Controller.h"

using namespace std;

Game::Game() {
    //gameClockMs = 0;
    hasSelection = false;
	selectedPiece = Position(-1, -1);
   /* moveInProgress = false;
    remainingMoveTime = 0;*/
	gameStatus = GameStatus::Playing;

    /*gameClockMsJ = 0;
    hasSelectionJ = false;
    selectedPieceJ = Position(-1, -1);
    jumpInProgress = false;
    remainingJumpTime = 0;*/
}

bool Game::isGameActive() const 
{ 
    return gameStatus == GameStatus::Playing; 
}

bool Game::gethasSelection() const
{
	return hasSelection;
}
Position Game::getSelectedPiece() const
{
	return selectedPiece;
}

void Game::setHasSelection(bool value)
{
	hasSelection = value;
}
void Game::setSelectedPiece(Position pos)
{
	selectedPiece = pos;
}

void Game::setupBoard(IBoardConvert& converter) {
    /*board.parse(boardLines);*/
    if (!converter.Convert(board))
        cout << board.getError() << endl;
}

//bool Game::isBoardValid() const {
//    return board.validate();
//}

//string Game::getBoardError() const {
//    return board.getError();
//}



void Game::run(std::vector<Command> commands) {
    /*std::vector<std::string> boardLines;
    std::vector<std::string> commandLines;
    bool parsingCommands = false;

    for (const auto& line : inputLines) {
        if (line.empty()) continue;

        if (line == "Commands:") {
            parsingCommands = true;
            continue;
        }
        if (line == " Board:") {
            continue;
        }

        if (!parsingCommands) {
            boardLines.push_back(line);
        }
        else {
            commandLines.push_back(line);
        }
    }*/

   /* std::cout << "DEBUG: Loaded board lines count: " << boardLines.size() << std::endl;
    for (const auto& line : boardLines) {
        std::cout << "DEBUG: Board line: [" << line << "]" << std::endl;
    }*/

    // 1. טעינת הלוח
    /*if (!boardLines.empty()) {
		TextBoardConvert converter(boardLines);
        setupBoard(converter);
    }*/

  
    /*StringCommandConvert converter(commandLines);
    std::vector<Command> commands = converter.Convert();*/

    for (const auto& cmd : commands) {
        switch (cmd.type) {
        case CommandType::Click:
            controller.executeClick(cmd.x, cmd.y, *this, this->arbiter);
            break;
        case CommandType::Jump:
            controller.executeJump(cmd.x, cmd.y, *this, this->arbiter);
			break;
        case CommandType::Wait:
        {
            RuleEngine ruleEngine;
            this->arbiter.tick(cmd.ms, this->board, ruleEngine, *this);
            break;
        }
        case CommandType::Print:
            TextualBoardPrinting printer;
            board.print(printer);
            break;
        }
    }
}

//void Game::executeClick(int x, int y)
//{
 //   if (gameStatus == GameStatus::BlackWins || 
 //       gameStatus == GameStatus::WhiteWins || 
 //       moveInProgress)
 //       return;

 //   Position place = BoardMapper::pixelToCell(x, y);

 //   if (!board.isInside(place))
 //       return;

 //   // אין כלי מסומן - בוחרים כלי
 //   if (!hasSelection)
 //   {
 //       if (!board.isEmpty(place) && 
 //           board.getPiece(place)->getStatus() == PieceStatus::idle)
 //       {
 //           hasSelection = true;
 //           selectedPiece = place;
 //       }
 //       return;
 //   }

 //   // לחיצה על כלי מאותו צבע - מחליפים בחירה
 //   if (!board.isEmpty(place) &&
 //       board.getPieceColor(place) ==
 //       board.getPieceColor(Position(selectedPiece.getRow(), selectedPiece.getCol())))
 //   {
 //       selectedPiece = place;
 //       return;
 //   }

 //   // בדיקת חוקיות
 //   if (!RuleEngine::isLegalMove(board, selectedPiece, place))
 //   {
 //       return;
 //   }

 //   // התחלת המהלך
 //   //moveInProgress = true;
 //   long long remainingMoveTime =
 //       std::max(abs(to.getRow() - from.getRow()),
 //           abs(to.getCol() - from.getCol())) * 1000;

	//arbiter.addAction(selectedPiece, place, remainingMoveTime);

	//board.getPiece(selectedPiece)->setStatus(PieceStatus::moving);

	///*from = selectedPiece;

	//to = place;

 //   */

 //   hasSelection = false;
//}

void Game::gameOver(Board& board, Position place)
{
    if (board.getPiece(place) && board.getPieceType(place) == PieceType::King)
    {
		board.getPieceColor(place) == PieceColor::White ? gameStatus = GameStatus::BlackWins : gameStatus = GameStatus::WhiteWins;
    }
}

//void Game::executeWait(long long ms)
//{
//    if (ms <= 0)
//        return;
//
//    gameClockMs += ms;
//
//    if (moveInProgress)
//    { 
//        remainingMoveTime -= ms;
//
//        if (remainingMoveTime <= 0)
//        {
//            if (board.getPiece(to) && 
//                board.getPiece(to)->getStatus() != PieceStatus::airborne
//                || board.getPiece(to) == nullptr)
//            {
//                gameOver(board, to);
//                board.movePiece(
//                    from,
//                    to);
//
//                if (RuleEngine::canPromote(board, to))
//                {
//                    board.promotePiece(to, PieceType::Queen);
//                }
//
//                board.getPiece(to)->setStatus(PieceStatus::idle);
//            }
//    
//		    board.removePiece(from);
//            moveInProgress = false;
//        }
//    }
//
//    if (jumpInProgress)
//    {
//        remainingJumpTime -= ms;
//        if (remainingJumpTime <= 0)
//        {
//            // בסיום 1000ms הקפיצה מסתיימת והכלי חוזר ל-idle
//            Piece* p = board.getPiece(from); // from מכיל את המיקום של הכלי הקופץ
//            if (p && p->getStatus() == PieceStatus::airborne)
//            {
//                p->setStatus(PieceStatus::idle);
//            }
//            jumpInProgress = false;
//        }
//    }
//}

//void Game::printBoard(IBoardPrinter& printer) const {
//    board.print(printer);
//}
//
//
//void Game::executeJump(int x, int y) {
	//Position place = BoardMapper::pixelToCell(x, y);
 //   Piece* p = board.getPiece(place);

 //   // בדיקה: האם הכלי קיים, לא נע, ולא נאכל?
 //   if (p && p->getStatus() == PieceStatus::idle) {
	//	//jumpInProgress = true;
 //

 //       arbiter.addAction(selectedPiece, place, 1000);
 //       p->setStatus(PieceStatus::airborne);
 //       remainingJumpTime = 1000;
	//	//from = place;
 //   }
//}
const Board& Game::getBoard() const
{
    return board;
}