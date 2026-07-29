#include "CreateRoomMessage.h"

#include "MiniJson.h"

namespace CreateRoomMessage
{
    std::string build()
    {
        return "{\"type\":\"createRoom\"}";
    }

    bool parse(const std::string& rawMessage)
    {
        std::string type;
        return MiniJson::extractStringField(rawMessage, "type", type) && type == "createRoom";
    }
}
