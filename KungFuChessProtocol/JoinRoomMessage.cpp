#include "JoinRoomMessage.h"

#include "MiniJson.h"

namespace JoinRoomMessage
{
    std::string build(const std::string& code)
    {
        return "{\"type\":\"joinRoom\",\"code\":\"" + code + "\"}";
    }

    bool parse(const std::string& rawMessage, std::string& outCode)
    {
        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "joinRoom")
            return false;

        return MiniJson::extractStringField(rawMessage, "code", outCode);
    }
}
