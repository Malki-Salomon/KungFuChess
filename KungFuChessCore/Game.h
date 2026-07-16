#pragma once
#include "Board.h"
#include "GameStatus.h"
#include "IBoardConvert.h"
#include "Position.h"
#include "RealTimeArbiter.h"
#include <string>
#include <vector>
#include "Controller.h"
#include "Command.h"


class Board;
class IBoardPrinter;

class Game {
private:
    Board board;
    //long long gameClockMs;
    bool hasSelection;
	Position selectedPiece;
    /*bool moveInProgress;
    Position from;
    Position to;
    long long remainingMoveTime;*/
    GameStatus gameStatus;
    Controller controller;
    /*long long gameClockMsJ;
    bool hasSelectionJ;
    Position selectedPieceJ;
    bool jumpInProgress;
    Position fromJ;
    Position toJ;
    long long remainingJumpTime;*/
    RealTimeArbiter arbiter;

public:
    Game();
	bool gethasSelection() const;
	Position getSelectedPiece() const;
	void setHasSelection(bool value);
	void setSelectedPiece(Position pos);
    void setupBoard(IBoardConvert& converter);
    bool isGameActive() const;
        /*bool isBoardValid() const;
        std::string getBoardError() const;*/
    void run(std::vector<Command> commands);
    //void executeClick(int x, int y);
    //void executeWait(long long ms);
    //void printBoard(IBoardPrinter& printer) const;
    //void executeJump(int x, int y);


    const Board& getBoard() const;

    void gameOver(Board& board, Position place);
};