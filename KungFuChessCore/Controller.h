#pragma once
#include "Board.h"
#include "RealTimeArbiter.h"
class game;

class Controller
{
private:
    Board board;

public:

    // col/row are board matrix cells, not pixels - the caller (GUI's Layout,
    // or a server client speaking the same "click col row" protocol) is
    // responsible for the pixel-to-cell conversion.
    void executeClick(int col, int row, Game& game, RealTimeArbiter& arbiter);
    void executeJump(int col, int row, Game& game, RealTimeArbiter& arbiter);
};

