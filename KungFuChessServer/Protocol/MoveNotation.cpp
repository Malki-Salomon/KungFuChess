#include "MoveNotation.h"

#include <cctype>

namespace MoveNotation
{
    bool squareToPosition(const std::string& square, int& outRow, int& outCol)
    {
        if (square.size() != 2)
            return false;

        char file = static_cast<char>(std::tolower(static_cast<unsigned char>(square[0])));
        char rank = square[1];

        if (file < 'a' || file > 'h')
            return false;
        if (rank < '1' || rank > '8')
            return false;

        outCol = file - 'a';
        outRow = 8 - (rank - '0');
        return true;
    }

    std::string toClickCommand(int row, int col)
    {
        // "click X Y" is parsed by Core as x=col, y=row (StringCommandConvert
        // reads "click <x> <y>", then Controller::executeClick(int col, int
        // row, ...) receives them in that order) - no pixel scaling.
        return "click " + std::to_string(col) + " " + std::to_string(row);
    }
}
