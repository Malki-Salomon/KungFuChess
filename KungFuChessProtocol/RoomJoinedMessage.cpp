#include "RoomJoinedMessage.h"

#include "MiniJson.h"

namespace RoomJoinedMessage
{
    std::string build(const std::string& code, const std::string& role)
    {
        return "{\"type\":\"roomJoined\",\"code\":\"" + code + "\",\"role\":\"" + role + "\"}";
    }

    bool parse(const std::string& rawMessage, std::string& outCode, std::string& outRole)
    {
        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "roomJoined")
            return false;

        if (!MiniJson::extractStringField(rawMessage, "code", outCode))
            return false;

        return MiniJson::extractStringField(rawMessage, "role", outRole);
    }
}
