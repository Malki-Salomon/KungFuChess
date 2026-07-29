#pragma once

#include <string>

// {"type":"assigned","color":"white"} - sent once, right after a
// connection's login succeeds, telling it which seat it holds ("white",
// "black", or "spectator"). Plain strings only - zero Core dependency,
// same reasoning as every other message in this library (see MoveMessage
// for why); the Seat -> color name translation itself lives in
// KungFuChessServer (see SeatColorNaming), which already legitimately
// depends on Core.
namespace AssignedMessage
{
    std::string build(const std::string& color);

    // Returns false (leaving outColor untouched) if this isn't a valid
    // assigned message.
    bool parse(const std::string& rawMessage, std::string& outColor);
}
