#pragma once

#include "GameWindow.h"
#include "IGameController.h"

// Drives the game's runtime: ticks the controller and paces the loop.
// Deliberately has no knowledge of mouse input or window/callback
// management - that's owned entirely by InputHandler.
class GameLoop
{
public:
    GameLoop(IGameController& gameController, GameWindow& gameWindow);

    void run();

private:
    IGameController& gameController;
    GameWindow& gameWindow;
};