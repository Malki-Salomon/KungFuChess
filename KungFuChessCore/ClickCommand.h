#pragma once

#include "Command.h"
#include <memory>
#include <istream>

class ClickCommand : public Command
{
private:
    int x;
    int y;

public:
    ClickCommand(int x, int y);

    void execute(Game& game) override;
};