#include "OpponentReconnectedMessage.h"

#include "MiniJson.h"

namespace OpponentReconnectedMessage
{
    std::string build()
    {
        return "{\"type\":\"opponentReconnected\"}";
    }

    bool parse(const std::string& rawMessage)
    {
        std::string type;
        return MiniJson::extractStringField(rawMessage, "type", type) && type == "opponentReconnected";
    }
}
