#pragma once

#include <string>

// {"type":"roomJoinFailed","reason":"room not found"} - the server's
// reply to a JoinRoomMessage naming a code that doesn't exist.
namespace RoomJoinFailedMessage
{
    std::string build(const std::string& reason);

    // Returns false (leaving outReason untouched) if this isn't a valid
    // roomJoinFailed message.
    bool parse(const std::string& rawMessage, std::string& outReason);
}
