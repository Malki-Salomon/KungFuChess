#include "GameOverMessage.h"

#include "MiniJson.h"

#include <sstream>

namespace GameOverMessage
{
    std::string build(const std::string& result, bool hasRatings, int whiteNewRating, int blackNewRating)
    {
        std::ostringstream out;
        out << "{\"type\":\"gameOver\",\"result\":\"" << result << "\"";

        if (hasRatings)
        {
            out << ",\"whiteNewRating\":" << whiteNewRating
                << ",\"blackNewRating\":" << blackNewRating;
        }

        out << "}";
        return out.str();
    }

    bool parse(const std::string& rawMessage, std::string& outResult, bool& outHasRatings, int& outWhiteNewRating, int& outBlackNewRating)
    {
        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "gameOver")
            return false;

        if (!MiniJson::extractStringField(rawMessage, "result", outResult))
            return false;

        int whiteRating = 0;
        int blackRating = 0;
        outHasRatings = MiniJson::extractIntField(rawMessage, "whiteNewRating", whiteRating)
            && MiniJson::extractIntField(rawMessage, "blackNewRating", blackRating);

        if (outHasRatings)
        {
            outWhiteNewRating = whiteRating;
            outBlackNewRating = blackRating;
        }

        return true;
    }
}
