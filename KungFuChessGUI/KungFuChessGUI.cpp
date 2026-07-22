// KungFuChessGUI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "img.hpp"
#include "IGameController.h"
//#include "GameSnapshot.h"
#include "GameWindow.h"
#include "PrinterAdapter.h"
#include <iostream>
#include <string>
#include "InputHandler.h"
#include "GameLoop.h"

namespace
{
    // Standard chess starting position, in the same text notation used by the
    // Core/Test board format: "<w|b><K|Q|R|B|N|P>" or "." for an empty square.
    // Row 0 is White's back rank, row 7 is Black's.
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
            "wR wN wB wQ wK wB wN wR",
            "Commands:",
            "print board" 
        };
    }
}

int main()
{
    try {
        auto app = CoreFactory::createGameController();

        // Uniquely identifies this session's display window. In a
        // multi-session server this would be the session/game ID handed to
        // each per-connection setup instead of a fixed literal, so that
        // concurrent sessions never collide on the same OpenCV window name.
        const std::string windowName = "GameWindow_Session1";

        GameWindow gameWindow(*app, windowName);
        InputHandler inputHandler(*app, gameWindow.getLayout(), gameWindow.getWindowName(), gameWindow.getMoveIntentHint());
        PrinterAdapter printer(gameWindow);
        app->setOutputDevice(&printer);
        app->parseLoad(buildStartingBoardText());

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
