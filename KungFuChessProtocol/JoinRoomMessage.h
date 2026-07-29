#pragma once

#include <string>

// {"type":"joinRoom","code":"3"} - sent by a logged-in client that wants
// to join (as a player, or as a spectator if two players already hold the
// room's seats) an existing room by its code.
namespace JoinRoomMessage
{
    std::string build(const std::string& code);

    // Returns false (leaving outCode untouched) if this isn't a valid
    // joinRoom message.
    bool parse(const std::string& rawMessage, std::string& outCode);
}
