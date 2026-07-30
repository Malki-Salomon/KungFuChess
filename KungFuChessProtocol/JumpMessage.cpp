#include "JumpMessage.h"

#include "MiniJson.h"

namespace JumpMessage
{
    std::string build(const std::string& square)
    {
        // No escaping needed: callers are expected to pass an algebraic
        // square only ("e2"), never arbitrary text.
        return "{\"type\":\"jump\",\"square\":\"" + square + "\"}";
    }

    bool parse(const std::string& rawMessage, std::string& outSquare)
    {
        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "jump")
            return false;

        return MiniJson::extractStringField(rawMessage, "square", outSquare);
    }
}
