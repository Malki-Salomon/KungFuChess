#include "pch.h"
#include "Bishop.h"
#include "Game.h"
#include "King.h"
#include "Knight.h"
#include "Pawn.h"
#include "Queen.h"
#include "Rook.h"
#include "TextBoardConvert.h"
#include <iostream>
#include <memory>
#include "RuleEngine.h"
#include "StringCommandConvert.h"
#include "Position.h"

using namespace std;

Game::Game() {
    gameClockMs = 0;
    hasSelection = false;
	selectedPiece = Position(-1, -1);
    moveInProgress = false;
    remainingMoveTime = 0;
	gameStatus = GameStatus::Playing;

    gameClockMsJ = 0;
    hasSelectionJ = false;
    selectedPieceJ = Position(-1, -1);
    jumpInProgress = false;
    remainingJumpTime = 0;
}

bool Game::isGameActive() const 
{ 
    return gameStatus == GameStatus::Playing; 
}

void Game::setupBoard(const vector<string>& boardLines) {
    /*board.parse(boardLines);*/
    TextBoardConvert converter(boardLines);
    if (!converter.Convert(board))
        cout << board.getError() << endl;
}

//bool Game::isBoardValid() const {
//    return board.validate();
//}

//string Game::getBoardError() const {
//    return board.getError();
//}



void Game::runSimulation(const std::vector<std::string>& inputLines) {
    std::vector<std::string> boardLines;
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
    }

   /* std::cout << "DEBUG: Loaded board lines count: " << boardLines.size() << std::endl;
    for (const auto& line : boardLines) {
        std::cout << "DEBUG: Board line: [" << line << "]" << std::endl;
    }*/

    // 1. טעינת הלוח
    if (!boardLines.empty()) {
        setupBoard(boardLines);
    }

  
    StringCommandConvert converter(commandLines);
    std::vector<Command> commands = converter.Convert();

    for (const auto& cmd : commands) {
        switch (cmd.type) {
        case CommandType::Click:
            this->executeClick(cmd.x, cmd.y);
            break;
        case CommandType::Jump:
            this->executeJump(cmd.x, cmd.y);
			break;
        case CommandType::Wait:
            this->executeWait(cmd.ms);
            break;
        case CommandType::Print:
            this->printBoard();
            break;
        }
    }
}

void Game::executeClick(int x, int y)
{
    if (gameStatus == GameStatus::BlackWins || 
        gameStatus == GameStatus::WhiteWins || 
        moveInProgress)
        return;

    if (x <  0 || y <  0)
        return;

    int c = x / 100;
    int r = y / 100;

    if (!board.isInside(Position(r, c)))
        return;

    // אין כלי מסומן - בוחרים כלי
    if (!hasSelection)
    {
		Position place = Position(r, c);
        if (!board.isEmpty(place) && 
            board.getPiece(place)->getStatus() == PieceStatus::idle)
        {
            hasSelection = true;
            selectedPiece.setRow(r);
            selectedPiece.setCol(c);
        }
        return;
    }

    // לחיצה על כלי מאותו צבע - מחליפים בחירה
    if (!board.isEmpty(Position(r, c)) &&
        board.getPieceColor(Position(r, c)) ==
        board.getPieceColor(Position(selectedPiece.getRow(), selectedPiece.getCol())))
    {
        selectedPiece.setRow(r);
        selectedPiece.setCol(c);
        return;
    }

    // בדיקת חוקיות
    if (!RuleEngine::isLegalMove(board, selectedPiece, Position(r, c)))
    {
        return;
    }

    // התחלת המהלך
    moveInProgress = true;

	board.getPiece(selectedPiece)->setStatus(PieceStatus::moving);

	from = selectedPiece;

    to.setRow(r);
    to.setCol(c);

    remainingMoveTime =
        std::max(abs(to.getRow() - from.getRow()),
            abs(to.getCol() - from.getCol())) * 1000;

    hasSelection = false;
}

void Game::gameOver(Board& board, Position place)
{
    if (board.getPiece(place) && board.getPieceType(place) == PieceType::King)
    {
		board.getPieceColor(place) == PieceColor::White ? gameStatus = GameStatus::BlackWins : gameStatus = GameStatus::WhiteWins;
    }
}

void Game::executeWait(long long ms)
{
    if (ms <= 0)
        return;

    gameClockMs += ms;

    if (moveInProgress)
    { 
        remainingMoveTime -= ms;

        if (remainingMoveTime <= 0)
        {
            if (board.getPiece(to) && 
                board.getPiece(to)->getStatus() != PieceStatus::airborne
                || board.getPiece(to) == nullptr)
            {
                gameOver(board, to);
                board.movePiece(
                    from,
                    to);

                if (RuleEngine::canPromote(board, to))
                {
                    board.promotePiece(to, PieceType::Queen);
                }

                board.getPiece(to)->setStatus(PieceStatus::idle);
            }
    
		    board.removePiece(from);
            moveInProgress = false;
        }
    }

    if (jumpInProgress)
    {
        remainingJumpTime -= ms;
        if (remainingJumpTime <= 0)
        {
            // בסיום 1000ms הקפיצה מסתיימת והכלי חוזר ל-idle
            Piece* p = board.getPiece(from); // from מכיל את המיקום של הכלי הקופץ
            if (p && p->getStatus() == PieceStatus::airborne)
            {
                p->setStatus(PieceStatus::idle);
            }
            jumpInProgress = false;
        }
    }
}

void Game::printBoard() const {
    board.print();
}


void Game::executeJump(int x, int y) {
    int c = x / 100;
    int r = y / 100;
    Piece* p = board.getPiece(Position(r, c));

    // בדיקה: האם הכלי קיים, לא נע, ולא נאכל?
    if (p && p->getStatus() == PieceStatus::idle) {
		jumpInProgress = true;
        p->setStatus(PieceStatus::airborne);
        remainingJumpTime = 1000;
        from.setRow(r); // נשמור את המיקום לצורך ניקוי בסוף
        from.setCol(c);
    }
}
const Board& Game::getBoard() const
{
    return board;
}