#pragma once

#include <string>

// {"type":"createRoom"} - sent by a logged-in client that wants a fresh
// game created. Carries no fields beyond "type" - the server replies with
// RoomCreatedMessage's code, which the client then joins explicitly via
// JoinRoomMessage (creating a room does not, by itself, seat the creator
// in it).
namespace CreateRoomMessage
{
    std::string build();

    // Returns false if this isn't a valid createRoom message.
    bool parse(const std::string& rawMessage);
}
