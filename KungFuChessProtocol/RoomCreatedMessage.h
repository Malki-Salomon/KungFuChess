#pragma once

#include <string>

// {"type":"roomCreated","code":"3"} - the server's reply to a successful
// CreateRoomMessage, carrying the new room's code. The creator is not yet
// a member of that room - see CreateRoomMessage's header.
namespace RoomCreatedMessage
{
    std::string build(const std::string& code);

    // Returns false (leaving outCode untouched) if this isn't a valid
    // roomCreated message.
    bool parse(const std::string& rawMessage, std::string& outCode);
}
