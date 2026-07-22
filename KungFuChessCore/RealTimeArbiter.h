#pragma once
#include "Position.h"
#include <memory>
#include <vector>

class Board;
class RuleEngine;
class Game;

struct ActiveAction
{
    Position from;
    Position to;
    long long remainingTime;
};

class RealTimeArbiter
{
public:
    void addAction(Position from, Position to, long long duration);
    bool tick(long long ms, Board& board, RuleEngine& rules, Game& game);
    bool hasActiveActions() const { return !activeActions.empty(); }

private:
    std::vector<ActiveAction> activeActions;
};
