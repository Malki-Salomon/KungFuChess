#include "MoveNotation.h"

namespace MoveNotation
{
    std::string toClickCommand(int row, int col)
    {
        // "click X Y" is parsed by Core as x=col, y=row (StringCommandConvert
        // reads "click <x> <y>", then Controller::executeClick(int col, int
        // row, ...) receives them in that order) - no pixel scaling.
        return "click " + std::to_string(col) + " " + std::to_string(row);
    }

    std::string toJumpCommand(int row, int col)
    {
        // Same col-then-row argument order as toClickCommand - Core's
        // Controller::executeJump(int col, int row, ...) mirrors
        // executeClick's signature exactly.
        return "jump " + std::to_string(col) + " " + std::to_string(row);
    }
}
