#include "RoomJoinFailedMessage.h"

#include "MiniJson.h"

namespace RoomJoinFailedMessage
{
    std::string build(const std::string& reason)
    {
        return "{\"type\":\"roomJoinFailed\",\"reason\":\"" + reason + "\"}";
    }

    bool parse(const std::string& rawMessage, std::string& outReason)
    {
        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "roomJoinFailed")
            return false;

        return MiniJson::extractStringField(rawMessage, "reason", outReason);
    }
}
