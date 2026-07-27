#pragma once

#include <string>
#include <vector>

// Result of parsing a raw client message into board coordinates.
// valid=false means malformed input or not a "move" message - callers
// should just ignore it in that case.
struct ParsedMove
{
    bool valid = false;
    int fromRow = 0, fromCol = 0;
    int toRow = 0, toCol = 0;
};

// Translates a raw incoming client message into the Core command(s) needed
// to carry it out. Currently supports exactly one message shape:
//   {"type":"move","from":"e2","to":"e4"}
// Split into parse + build so a caller (Server) can check move ownership
// against the parsed "from" square before committing to dispatching
// anything into Core.
namespace MoveTranslator
{
    ParsedMove parseMove(const std::string& rawMessage);

    // Builds the two "click X Y" commands (select, then target) for an
    // already-parsed move - matching how a real mouse-click sequence would
    // drive the same move. Caller should only call this for a valid move.
    std::vector<std::string> toClickCommands(const ParsedMove& move);

    // Convenience wrapper (parseMove + toClickCommands) for callers that
    // don't need to inspect the parsed move first, e.g. tests. Returns an
    // empty vector for anything invalid.
    std::vector<std::string> translateMoveMessage(const std::string& rawMessage);
}
