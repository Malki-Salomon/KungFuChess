#pragma once

#include "GameWindow.h"

// Drives the GUI's presentation runtime: pumps the window's event loop,
// advances animation interpolation, and paces frames.
//
// Runs no game logic at all - the game itself lives entirely server-side,
// and this process only ever displays snapshots it is sent. Board state
// arrives asynchronously on the network receive thread (see
// NetworkSnapshotAdapter) rather than being produced here, so this loop
// has nothing to tick but the animations.
//
// Deliberately has no knowledge of mouse input or window/callback
// management - that's owned entirely by InputHandler.
class GameLoop
{
public:
    explicit GameLoop(GameWindow& gameWindow);

    void run();

private:
    GameWindow& gameWindow;
};
