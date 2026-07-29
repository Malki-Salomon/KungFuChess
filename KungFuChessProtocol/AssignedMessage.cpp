#include "AssignedMessage.h"

#include "MiniJson.h"

namespace AssignedMessage
{
    std::string build(const std::string& color)
    {
        return "{\"type\":\"assigned\",\"color\":\"" + color + "\"}";
    }

    bool parse(const std::string& rawMessage, std::string& outColor)
    {
        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "assigned")
            return false;

        return MiniJson::extractStringField(rawMessage, "color", outColor);
    }
}
