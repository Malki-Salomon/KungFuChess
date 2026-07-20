#pragma once
#include "Board.h"
#include "Command.h"
#include "Controller.h"
#include "GameSnapshot.h"
#include "GameStatus.h"
#include "IBoardConvert.h"
#include "Position.h"
#include "RealTimeArbiter.h"
#include <string>
#include <vector>
#include <queue>

class Board;
class IBoardPrinter;

class Game
{
public:
    Game();

    void setupBoard(IBoardConvert& converter);
    void addCommand(Command cmd);
    void processNextCommand();
    bool hasCommands() const;
    void gameOver(Board& board, Position place);

    bool isGameActive() const;
    const Board& getBoard() const;
    GameSnapshot getSnapshot() const;

    bool gethasSelection() const;
    Position getSelectedPiece() const;
    void setHasSelection(bool value);
    void setSelectedPiece(Position pos);
	void setPrinter(IBoardPrinter* printer);

private:
    Board board;
    bool hasSelection;
    Position selectedPiece;
    GameStatus gameStatus;
    Controller controller;
    RealTimeArbiter arbiter;
    std::queue<Command> commandQueue;
	IBoardPrinter* printer;
};
