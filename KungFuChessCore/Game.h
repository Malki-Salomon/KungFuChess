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

public:
    Game();

    void setupBoard(const std::vector<std::string>& boardLines);
    bool isBoardValid() const;
    std::string getBoardError() const;

    void executeClick(int x, int y);
    void executeWait(long long ms);
    void printBoard() const;
};