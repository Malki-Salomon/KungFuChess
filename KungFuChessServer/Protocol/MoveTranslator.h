#pragma once

#include <string>
#include <vector>

// Translates a raw incoming client message into the Core command(s) needed
// to carry it out. Currently supports exactly one message shape:
//   {"type":"move","from":"e2","to":"e4"}
// which becomes two "click X Y" commands (select, then target) - matching
// how a real mouse-click sequence would drive the same move.
namespace MoveTranslator
{
    // Returns the Core commands to dispatch, in order. Returns an empty
    // vector for anything malformed or not a recognized message type -
    // safe to just skip in that case (logged by the caller, not here).
    std::vector<std::string> translateMoveMessage(const std::string& rawMessage);
}
