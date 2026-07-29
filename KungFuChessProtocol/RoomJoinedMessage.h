#pragma once

#include <string>

// {"type":"roomJoined","code":"3","role":"player"} - the server's reply
// to a successful JoinRoomMessage. "role" is "player" if the join got a
// real seat (White/Black), or "spectator" if the room's two seats were
// already taken.
namespace RoomJoinedMessage
{
    std::string build(const std::string& code, const std::string& role);

    // Returns false (leaving the out-params untouched) if this isn't a
    // valid roomJoined message.
    bool parse(const std::string& rawMessage, std::string& outCode, std::string& outRole);
}
