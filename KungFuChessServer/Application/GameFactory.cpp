#include "GameFactory.h"

#include "IGameController.h" // also declares the CoreFactory namespace

#include <string>
#include <vector>

namespace
{
    // Standard chess starting position, in the same text notation used by the
    // Core/Test board format: "<w|b><K|Q|R|B|N|P>" or "." for an empty square.
    // Row 0 is White's back rank, row 7 is Black's.
    // NOTE: the leading space before "Board:" is required - App::parseLoad
    // matches this exact literal to know where the board section starts.
    // NOTE: no "Commands:" section here on purpose. The server has no
    // printer attached (setOutputDevice() is never called on this
    // controller), and there's nothing useful for it to render yet anyway.
    // Game guards against a null printer internally now, so this wouldn't
    // crash even with a "print board" command left in - it's simply not
    // needed here. The server starts with an empty command queue; it will
    // be fed commands from clients once the Network layer exists.
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

namespace GameFactory
{
    std::unique_ptr<IGameController> createStandardGame()
    {
        auto controller = CoreFactory::createGameController();
        controller->parseLoad(buildStartingBoardText());
        return controller;
    }
}
