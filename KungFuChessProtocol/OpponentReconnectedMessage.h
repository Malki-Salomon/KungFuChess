#pragma once

#include <string>

// {"type":"opponentReconnected"} - sent once to a room's remaining
// player(s) the moment their disconnected opponent successfully
// reconnects within the grace period.
namespace OpponentReconnectedMessage
{
    std::string build();

    // Returns false if this isn't a valid opponentReconnected message.
    bool parse(const std::string& rawMessage);
}
