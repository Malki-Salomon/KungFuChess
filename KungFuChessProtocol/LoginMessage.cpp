#include "LoginMessage.h"

#include "MiniJson.h"

namespace LoginMessage
{
    std::string build(const std::string& username, const std::string& password)
    {
        // No escaping - consistent with MiniJson's own documented
        // narrowness. A username/password containing a '"' would break
        // this; fine for now since nothing validates them at this stage
        // anyway.
        return "{\"type\":\"login\",\"username\":\"" + username + "\",\"password\":\"" + password + "\"}";
    }

    bool parse(const std::string& rawMessage, std::string& outUsername, std::string& outPassword)
    {
        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "login")
            return false;

        if (!MiniJson::extractStringField(rawMessage, "username", outUsername))
            return false;

        return MiniJson::extractStringField(rawMessage, "password", outPassword);
    }
}
