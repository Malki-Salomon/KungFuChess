#pragma once
#include <iostream>
#include <memory>

class Game;

class Command
{
public:
    virtual ~Command() = default;

    virtual void execute(Game& game) = 0;
};