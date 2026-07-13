#pragma once
#include "Position.h"

class Board;

class RuleEngine
{
public:
	static bool isPathClear(Board& board, Position from, Position to);
	static bool isLegalMove(Board& board, Position from, Position to);
};

