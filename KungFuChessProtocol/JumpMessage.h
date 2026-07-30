#pragma once

#include <string>

// {"type":"jump","square":"e2"} - what a client sends to attempt a jump:
// the piece on that square launches straight up and comes back down on the
// same square (see Controller::executeJump, Core-side), briefly going
// airborne rather than travelling anywhere.
//
// Deliberately ONE square, not from/to like MoveMessage - a jump has no
// destination, which is exactly what distinguishes it from a move at the
// protocol level rather than leaving clients to encode it as some special
// same-square move.
//
// This is the ONE place that shape is defined - both build() (used by
// clients) and parse() (used by MoveTranslator, server-side) go through
// here, so the two sides can't silently drift apart the way independently
// hand-written build/parse code could.
namespace JumpMessage
{
    std::string build(const std::string& square);

    // Returns false (leaving outSquare untouched) if this isn't a valid
    // jump message. Only extracts the raw square text - whether that
    // square actually holds a jumpable piece is not this function's
    // concern (see MoveNotation/MoveTranslator, server-side).
    bool parse(const std::string& rawMessage, std::string& outSquare);
}
