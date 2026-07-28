#include "MiniJson.h"

namespace MiniJson
{
    bool extractStringField(const std::string& jsonText, const std::string& key, std::string& outValue)
    {
        std::string needle = "\"" + key + "\"";
        size_t keyPos = jsonText.find(needle);
        if (keyPos == std::string::npos)
            return false;

        size_t colonPos = jsonText.find(':', keyPos + needle.size());
        if (colonPos == std::string::npos)
            return false;

        size_t firstQuote = jsonText.find('"', colonPos + 1);
        if (firstQuote == std::string::npos)
            return false;

        size_t secondQuote = jsonText.find('"', firstQuote + 1);
        if (secondQuote == std::string::npos)
            return false;

        outValue = jsonText.substr(firstQuote + 1, secondQuote - firstQuote - 1);
        return true;
    }
}
