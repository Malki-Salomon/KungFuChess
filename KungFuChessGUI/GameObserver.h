#pragma once

struct GameSnapshot;

class GameObserver
{
public:
    virtual ~GameObserver() = default;

    virtual void update(const GameSnapshot& snapshot) = 0;
};
