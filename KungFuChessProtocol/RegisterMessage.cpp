#include "RegisterMessage.h"

#include "MiniJson.h"

namespace RegisterMessage
{
    std::string build(const std::string& username, const std::string& password)
    {
        // No escaping - see LoginMessage::build for why that's fine here.
        return "{\"type\":\"register\",\"username\":\"" + username + "\",\"password\":\"" + password + "\"}";
    }

    bool parse(const std::string& rawMessage, std::string& outUsername, std::string& outPassword)
    {
        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "register")
            return false;

        if (!MiniJson::extractStringField(rawMessage, "username", outUsername))
            return false;

        return MiniJson::extractStringField(rawMessage, "password", outPassword);
    }
}
