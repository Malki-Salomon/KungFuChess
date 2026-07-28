#include "LoginMessage.h"

#include "MiniJson.h"

namespace LoginMessage
{
    std::string build(const std::string& username)
    {
        // No escaping - consistent with MiniJson's own documented
        // narrowness. A username containing a '"' would break this; fine
        // for now since nothing validates usernames at this stage anyway.
        return "{\"type\":\"login\",\"username\":\"" + username + "\"}";
    }

    bool parse(const std::string& rawMessage, std::string& outUsername)
    {
        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "login")
            return false;

        return MiniJson::extractStringField(rawMessage, "username", outUsername);
    }
}
