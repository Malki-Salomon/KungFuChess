#pragma once

#include <string>

// Converts between standard algebraic squares ("e2") and the board
// row/col + pixel-click representation Core actually understands.
//
// Board orientation matches the starting-position text used throughout
// this project: row 0 = black's back rank (rank 8), row 7 = white's back
// rank (rank 1). So rank r -> row = 8 - r, file 'a'..'h' -> col 0..7.
namespace MoveNotation
{
    // Parses e.g. "e2" into board row/col. Returns false (leaving
    // outRow/outCol unchanged) if the text isn't a valid square.
    bool squareToPosition(const std::string& square, int& outRow, int& outCol);

    // Builds the "click X Y" command Core's StringCommandConvert expects.
    // As of the current Controller::executeClick(int col, int row, ...),
    // Core takes DIRECT board col/row here - NOT pixel coordinates. (This
    // changed at some point; Controller used to call
    // BoardMapper::pixelToCell() to convert pixels down to a cell, but that
    // conversion was removed from Core.)
    std::string toClickCommand(int row, int col);
}
