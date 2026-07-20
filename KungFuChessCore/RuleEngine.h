#pragma once
#include "Board.h"
#include "GameStatus.h"
#include "Position.h"

class Board;

class RuleEngine
{
public:
    static bool isLegalMove(Board& board, Position from, Position to);
    static bool isPathClear(Board& board, Position from, Position to);
    static bool canPromote(const Board& board, Position pos);
};
