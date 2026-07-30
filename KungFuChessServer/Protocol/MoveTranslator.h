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

// Result of parsing a raw client jump message into board coordinates.
// valid=false means malformed input or not a "jump" message - callers
// should just ignore it in that case. One square only, not from/to: a
// jumping piece lands back where it started (see JumpMessage).
struct ParsedJump
{
    bool valid = false;
    int row = 0, col = 0;
};

// Translates a raw incoming client message into the Core command(s) needed
// to carry it out. Supports two message shapes:
//   {"type":"move","from":"e2","to":"e4"}
//   {"type":"jump","square":"e2"}
// Split into parse + build so a caller (Server) can check ownership
// against the parsed square before committing to dispatching anything
// into Core.
namespace MoveTranslator
{
    ParsedMove parseMove(const std::string& rawMessage);

    ParsedJump parseJump(const std::string& rawMessage);

    // Builds the two "click X Y" commands (select, then target) for an
    // already-parsed move - matching how a real mouse-click sequence would
    // drive the same move. Caller should only call this for a valid move.
    std::vector<std::string> toClickCommands(const ParsedMove& move);

    // Builds the single "jump X Y" command for an already-parsed jump.
    // Caller should only call this for a valid jump.
    std::string toJumpCommand(const ParsedJump& jump);

    // Convenience wrapper (parseMove + toClickCommands) for callers that
    // don't need to inspect the parsed move first, e.g. tests. Returns an
    // empty vector for anything invalid.
    std::vector<std::string> translateMoveMessage(const std::string& rawMessage);
}
