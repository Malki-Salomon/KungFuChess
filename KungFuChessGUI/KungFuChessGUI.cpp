// KungFuChessGUI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "img.hpp"
#include "IGameController.h"
//#include "GameSnapshot.h"
#include "Events/EventBus.h"
#include "GameWindow.h"
#include "PrinterAdapter.h"
#include <iostream>
#include <string>
#include "InputHandler.h"
#include "GameLoop.h"
#include <opencv2/highgui.hpp>

namespace
{
    // Standard chess starting position, in the same text notation used by the
    // Core/Test board format: "<w|b><K|Q|R|B|N|P>" or "." for an empty square.
    // Row 0 is White's back rank, row 7 is Black's.
    // NOTE: no "Commands:" / "print board" section anymore - Game now
    // publishes the initial snapshot on its own as soon as it has both a
    // board and a printer (see Game::publishSnapshotIfReady), so the first
    // paint no longer depends on an explicit startup command.
    std::vector<std::string> buildStartingBoardText()
    {
        return
        {
            " Board:",
            "bR bN bB bQ bK bB bN bR",
            "bP bP bP bP bP bP bP bP",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            "wP wP wP wP wP wP wP wP",
            "wR wN wB wQ wK wB wN wR"
        };
    }
}

int main()
{
    try {
        auto app = CoreFactory::createGameController();

        // The sole notification channel between Core and any consumer
        // (GameWindow today; a future WebSocket relay, logger, etc. could
        // subscribe alongside it without App or Game changing at all).
        EventBus bus;

        // Uniquely identifies this session's display window. In a
        // multi-session server this would be the session/game ID handed to
        // each per-connection setup instead of a fixed literal, so that
        // concurrent sessions never collide on the same OpenCV window name.
        const std::string windowName = "GameWindow_Session1";

        GameWindow gameWindow(*app, windowName, bus);
        InputHandler inputHandler(*app, gameWindow.getLayout(), gameWindow.getWindowName(), gameWindow.getMoveIntentHint());
        PrinterAdapter printer(bus);
        app->setOutputDevice(&printer);
        app->parseLoad(buildStartingBoardText());

        // Game::setupBoard() just triggered the initial paint synchronously
        // (via the printer -> EventBus -> GameWindow::update chain), but
        // imshow() alone doesn't flush pixels to the screen - OpenCV only
        // does that when its window message loop gets pumped, which
        // otherwise wouldn't happen until GameLoop::run()'s first
        // cv::waitKey() call. Pump it once here so the starting board is
        // guaranteed to be visible before the loop even begins.
        cv::waitKey(1);

        GameLoop gameLoop(*app, gameWindow);
        gameLoop.run();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu