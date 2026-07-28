#include "MiniJson.h"

#include <cctype>

namespace MiniJson
{
    namespace
    {
        // Shared by extractBoolField/extractIntField: locate the colon
        // after "key", then skip whitespace to land on the first
        // character of the (unquoted) value. Returns std::string::npos if
        // the key isn't found at all.
        size_t findValueStart(const std::string& jsonText, const std::string& key)
        {
            std::string needle = "\"" + key + "\"";
            size_t keyPos = jsonText.find(needle);
            if (keyPos == std::string::npos)
                return std::string::npos;

            size_t colonPos = jsonText.find(':', keyPos + needle.size());
            if (colonPos == std::string::npos)
                return std::string::npos;

            size_t pos = colonPos + 1;
            while (pos < jsonText.size() && std::isspace(static_cast<unsigned char>(jsonText[pos])))
                ++pos;

            return pos;
        }
    }

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

    bool extractBoolField(const std::string& jsonText, const std::string& key, bool& outValue)
    {
        size_t pos = findValueStart(jsonText, key);
        if (pos == std::string::npos)
            return false;

        if (jsonText.compare(pos, 4, "true") == 0)
        {
            outValue = true;
            return true;
        }
        if (jsonText.compare(pos, 5, "false") == 0)
        {
            outValue = false;
            return true;
        }
        return false;
    }

    bool extractIntField(const std::string& jsonText, const std::string& key, int& outValue)
    {
        size_t pos = findValueStart(jsonText, key);
        if (pos == std::string::npos)
            return false;

        size_t start = pos;
        if (pos < jsonText.size() && jsonText[pos] == '-')
            ++pos;

        size_t digitsStart = pos;
        while (pos < jsonText.size() && std::isdigit(static_cast<unsigned char>(jsonText[pos])))
            ++pos;

        if (pos == digitsStart) // no digits found
            return false;

        outValue = std::stoi(jsonText.substr(start, pos - start));
        return true;
    }
}
