#include "MoveMessage.h"

#include "MiniJson.h"

namespace MoveMessage
{
    std::string build(const std::string& from, const std::string& to)
    {
        // No escaping needed: callers are expected to pass algebraic
        // squares only ("e2"), never arbitrary text.
        return "{\"type\":\"move\",\"from\":\"" + from + "\",\"to\":\"" + to + "\"}";
    }

    bool parse(const std::string& rawMessage, std::string& outFrom, std::string& outTo)
    {
        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "move")
            return false;

        if (!MiniJson::extractStringField(rawMessage, "from", outFrom))
            return false;
        if (!MiniJson::extractStringField(rawMessage, "to", outTo))
            return false;

        return true;
    }
}
