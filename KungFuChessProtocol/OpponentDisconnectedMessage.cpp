#include "OpponentDisconnectedMessage.h"

#include "MiniJson.h"

#include <sstream>

namespace OpponentDisconnectedMessage
{
    std::string build(int secondsRemaining)
    {
        std::ostringstream out;
        out << "{\"type\":\"opponentDisconnected\",\"secondsRemaining\":" << secondsRemaining << "}";
        return out.str();
    }

    bool parse(const std::string& rawMessage, int& outSecondsRemaining)
    {
        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "opponentDisconnected")
            return false;

        return MiniJson::extractIntField(rawMessage, "secondsRemaining", outSecondsRemaining);
    }
}
