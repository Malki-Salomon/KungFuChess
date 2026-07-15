#pragma once
#include "Position.h"
#include "GameStatus.h"
#include "Board.h"

class Board;

class RuleEngine
{
public:
	static bool isPathClear(Board& board, Position from, Position to);
	static bool isLegalMove(Board& board, Position from, Position to);
	//static GameStatus evaluate(const Board& board);
	static bool canPromote(const Board& board, Position pos);
};

