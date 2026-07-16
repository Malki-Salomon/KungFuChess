#pragma once
#include "Board.h"
#include "RealTimeArbiter.h"
class game;

class Controller
{
private:
    Board board;

public:

    void executeClick(int x, int y, Game& game, RealTimeArbiter& arbiter);
    void executeJump(int x, int y, Game& game, RealTimeArbiter& arbiter);
};

