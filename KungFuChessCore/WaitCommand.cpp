#include "pch.h"
#include "WaitCommand.h"
#include "CommandFactory.h"
#include "Game.h"

using namespace std;

WaitCommand::WaitCommand(long long ms)
    : ms(ms)
{
}

void WaitCommand::execute(Game& game)
{
    game.executeWait(ms);
}
