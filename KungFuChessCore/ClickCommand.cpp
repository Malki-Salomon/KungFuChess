#include "pch.h"
#include "ClickCommand.h"
#include "CommandFactory.h"
#include "Game.h"

using namespace std;

ClickCommand::ClickCommand(int x, int y)
    : x(x), y(y)
{
}

void ClickCommand::execute(Game& game)
{
    game.executeClick(x, y);
}
