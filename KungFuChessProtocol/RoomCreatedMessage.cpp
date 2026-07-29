#include "RoomCreatedMessage.h"

#include "MiniJson.h"

namespace RoomCreatedMessage
{
    std::string build(const std::string& code)
    {
        return "{\"type\":\"roomCreated\",\"code\":\"" + code + "\"}";
    }

    bool parse(const std::string& rawMessage, std::string& outCode)
    {
        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "roomCreated")
            return false;

        return MiniJson::extractStringField(rawMessage, "code", outCode);
    }
}
