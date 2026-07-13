#pragma once
#include "Board.h"
#include <string>
#include <vector>

class Game {
private:
    Board board;
    long long gameClockMs;
    bool hasSelection;
    int selectedRow;
    int selectedCol;

    bool moveInProgress;

    int moveFromRow;
    int moveFromCol;

    int moveToRow;
    int moveToCol;

    long long remainingMoveTime;

public:
    Game();

    void setupBoard(const std::vector<std::string>& boardLines);
    bool isBoardValid() const;
    std::string getBoardError() const;
    void runSimulation(const std::vector<std::string>& inputLines);
    void executeClick(int x, int y);
    void executeWait(long long ms);
    void printBoard() const;

    const Board& getBoard() const;
};