#pragma once

#include <string>

// {"type":"move","from":"e2","to":"e4"} - what a client sends to attempt a
// move. This is the ONE place that shape is defined - both build() (used
// by clients) and parse() (used by MoveTranslator, server-side) go through
// here, so the two sides can't silently drift apart the way independently
// hand-written build/parse code could.
namespace MoveMessage
{
    std::string build(const std::string& from, const std::string& to);

    // Returns false (leaving outFrom/outTo untouched) if this isn't a
    // valid move message. Only extracts the raw "from"/"to" square text -
    // whether those squares are actually legal is not this function's
    // concern (see MoveNotation/MoveTranslator, server-side).
    bool parse(const std::string& rawMessage, std::string& outFrom, std::string& outTo);
}
