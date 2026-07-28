#pragma once

#include <string>

// {"type":"login","username":"..."} - sent once by a client right after
// connecting. Username is for display/identification only in this stage -
// no password, no uniqueness check, no persistence.
namespace LoginMessage
{
    std::string build(const std::string& username);

    // Returns false (leaving outUsername untouched) if this isn't a valid
    // login message.
    bool parse(const std::string& rawMessage, std::string& outUsername);
}
