#pragma once

#include "Command.h"

class WaitCommand : public Command
{
private:
    long long ms;

public:
    WaitCommand(long long ms);

    void execute(Game& game) override;
};