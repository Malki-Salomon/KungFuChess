#pragma once

#include <string>

// Extracts a single string-valued field from a flat JSON object, e.g.
// pulling "e2" out of {"type":"move","from":"e2","to":"e4"}.
//
// This is NOT a general JSON parser - it only handles flat objects with
// string values, found via simple text search. It will misbehave on
// nested objects, arrays, non-string values, or a key name that happens
// to appear inside another field's string value. That's an intentional,
// documented limitation: this protocol's incoming messages are simple
// enough not to need a real parser yet. Swap in a proper JSON library
// (e.g. nlohmann::json) if the protocol grows beyond flat string fields.
namespace MiniJson
{
    // Returns false if the key isn't found or isn't a quoted string value.
    bool extractStringField(const std::string& jsonText, const std::string& key, std::string& outValue);
}
