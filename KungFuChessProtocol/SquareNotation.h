#pragma once

#include <string>

// The single source of truth for this protocol's board-coordinate
// convention, in both directions. Every participant - the GUI turning a
// click into a square, the server turning a received square back into a
// board cell - goes through here, so the two sides of the wire cannot
// drift apart the way two independently hand-written conversions could.
//
// The convention, used consistently throughout this project:
//
//   row 0 = black's back rank (rank 8), row 7 = white's back rank (rank 1)
//   col 0 = file 'a',                   col 7 = file 'h'
//
// so rank = 8 - row and file = 'a' + col. That also matches the GUI's
// Layout (row 0 drawn at the top of the window), which is what makes a raw
// click's cell usable here without any flipping.
//
// Coordinate conversion is this file's ONLY responsibility. It knows
// nothing about rendering, input, sessions, Core's Board, OpenCV or
// networking, and depends on nothing but <string> - which is what lets it
// sit at the bottom of the dependency graph where both clients and the
// server can reach it.
namespace Protocol
{
    // A board cell in the convention described above. Deliberately plain
    // data - no behavior, no invariants to maintain.
    struct Square
    {
        int row;
        int col;
    };

    class SquareNotation
    {
    public:
        // Returns e.g. "a8" for (0,0) and "h1" for (7,7).
        //
        // Returns an EMPTY STRING for any cell outside a standard 8x8
        // board, rather than composing nonsense text like "i9" that would
        // then travel over the wire. Callers that can produce out-of-range
        // cells should treat empty as "no such square".
        static std::string toNotation(int row, int col);

        // Parses e.g. "a8" into (row 0, col 0). Returns false - leaving
        // `square` untouched - for anything that isn't exactly one file
        // character a-h (either case) followed by one rank digit 1-8.
        static bool tryParse(const std::string& notation, Square& square);
    };
}
