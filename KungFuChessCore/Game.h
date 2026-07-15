#pragma once
#include "Board.h"
#include "GameStatus.h"
#include "IBoardConvert.h"
#include "Position.h"
#include <string>
#include <vector>

class Board;
class IBoardPrinter;

class Game {
private:
    Board board;
    long long gameClockMs;
    bool hasSelection;
	Position selectedPiece;
    bool moveInProgress;
    Position from;
    Position to;
    long long remainingMoveTime;
    GameStatus gameStatus;
    
    long long gameClockMsJ;
    bool hasSelectionJ;
    Position selectedPieceJ;
    bool jumpInProgress;
    Position fromJ;
    Position toJ;
    long long remainingJumpTime;

public:
    Game();

    void setupBoard(IBoardConvert& converter);
    bool isGameActive() const;
        /*bool isBoardValid() const;
        std::string getBoardError() const;*/
    void runSimulation(const std::vector<std::string>& inputLines);
    void executeClick(int x, int y);
    void executeWait(long long ms);
    void printBoard(IBoardPrinter& printer) const;
    void executeJump(int x, int y);


    const Board& getBoard() const;

    void gameOver(Board& board, Position place);
};