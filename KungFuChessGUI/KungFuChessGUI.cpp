// KungFuChessGUI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "img.hpp"
#include "App.h"
#include "GameSnapshot.h"
#include "GuiBoardPrinting.h"
#include <iostream>
#include <vector>

namespace
{
    // Standard chess starting position, in the same text notation used by the
    // Core/Test board format: "<w|b><K|Q|R|B|N|P>" or "." for an empty square.
    // Row 0 is White's back rank, row 7 is Black's.
    std::vector<std::string> buildStartingBoardText()
    {
        return {
            " Board:",
            "wR wN wB wQ wK wB wN wR",
            "wP wP wP wP wP wP wP wP",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            "bP bP bP bP bP bP bP bP",
            "bR bN bB bQ bK bB bN bR",
            "Commands:",
            "print board"
        };
    }
}

int main()
{
    try {
        App app;
		GuiBoardPrinting printer;
        app.setOutputDevice(&printer);
        app.parseLoad(buildStartingBoardText());
        app.run();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
