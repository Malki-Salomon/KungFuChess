#pragma once
#include <vector>
#include <memory>
#include "Position.h"

class Board;
class RuleEngine;
class Game;

struct ActiveAction {
    Position from;
    Position to;
    long long remainingTime;
};

class RealTimeArbiter {
private:
    std::vector<ActiveAction> activeActions;

public:
    void addAction(Position from, Position to, long long duration);

    void tick(long long ms, Board& board, RuleEngine& rules, Game& game);

    bool hasActiveActions() const { return !activeActions.empty(); }
};