#pragma once

#include <string>
#include <vector>

// {"type":"board","cells":[["bR","bN",...],["bP",...],...]} - a plain 8x8
// grid of symbol strings ("wP", "bR", "."). This is the ONE place that
// shape is defined - both build() (used server-side, by
// SnapshotBroadcaster) and parse() (used client-side, for display) go
// through here, so the two sides can't silently drift apart the way
// independently hand-written build/parse code could.
//
// Deliberately just strings, not PieceType/PieceColor - this file has no
// Core dependency. Converting Core's GameSnapshot into a grid of symbol
// strings is SnapshotBroadcaster's job, not this one's.
namespace BoardMessage
{
    // cells must be exactly 8 rows of 8 strings each.
    std::string build(const std::vector<std::vector<std::string>>& cells);

    // Returns false (leaving outRows untouched) if this isn't a valid
    // board message, or doesn't contain exactly 64 cell tokens.
    //
    // Purpose-built, like MiniJson - scans for quoted string tokens inside
    // the "cells" array in order and reshapes them into rows of 8, rather
    // than parsing the array nesting structurally.
    bool parse(const std::string& rawMessage, std::vector<std::vector<std::string>>& outRows);
}
