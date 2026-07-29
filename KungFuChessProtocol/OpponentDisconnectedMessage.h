#pragma once

#include <string>

// {"type":"opponentDisconnected","secondsRemaining":20} - sent once to a
// room's remaining player(s) the moment their opponent's connection
// drops mid-game, telling them how long the opponent has to reconnect
// before the game is decided by resignation.
namespace OpponentDisconnectedMessage
{
    std::string build(int secondsRemaining);

    // Returns false (leaving outSecondsRemaining untouched) if this isn't
    // a valid opponentDisconnected message.
    bool parse(const std::string& rawMessage, int& outSecondsRemaining);
}
