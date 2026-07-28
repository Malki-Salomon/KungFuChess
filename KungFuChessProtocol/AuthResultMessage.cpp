#include "AuthResultMessage.h"

#include "MiniJson.h"

#include <sstream>

namespace AuthResultMessage
{
    std::string build(bool success, const std::string& message, int rating)
    {
        // No escaping of message - fine because it's always one of a
        // small set of fixed, quote-free strings AuthService produces
        // (e.g. "wrong password"), never raw user input.
        std::ostringstream out;
        out << "{\"type\":\"authResult\",\"success\":" << (success ? "true" : "false")
            << ",\"message\":\"" << message << "\",\"rating\":" << rating << "}";
        return out.str();
    }

    bool parse(const std::string& rawMessage, bool& outSuccess, std::string& outMessage, int& outRating)
    {
        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "authResult")
            return false;

        if (!MiniJson::extractBoolField(rawMessage, "success", outSuccess))
            return false;

        if (!MiniJson::extractStringField(rawMessage, "message", outMessage))
            return false;

        return MiniJson::extractIntField(rawMessage, "rating", outRating);
    }
}
