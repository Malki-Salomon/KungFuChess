#pragma once

#include <string>

// Builds the command strings Core's StringCommandConvert parses, from
// board coordinates.
//
// Deliberately does NOT convert between algebraic squares ("e2") and board
// coordinates - that convention is shared with every client, so it lives
// in the protocol library (see Protocol::SquareNotation) rather than being
// re-implemented on the server side. This file's only responsibility is
// the server-specific half: turning coordinates into Core commands.
namespace MoveNotation
{
    // Builds the "click X Y" command Core's StringCommandConvert expects.
    // As of the current Controller::executeClick(int col, int row, ...),
    // Core takes DIRECT board col/row here - NOT pixel coordinates. (This
    // changed at some point; Controller used to call
    // BoardMapper::pixelToCell() to convert pixels down to a cell, but that
    // conversion was removed from Core.)
    std::string toClickCommand(int row, int col);

    // Builds the "jump X Y" command Core's StringCommandConvert expects.
    // Same col/row-not-pixels convention as toClickCommand above; a jump
    // names one square only, since the piece lands back where it started.
    std::string toJumpCommand(int row, int col);
}
