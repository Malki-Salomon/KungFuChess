#pragma once

#include <string>

// {"type":"register","username":"...","password":"..."} - sent by a
// client that wants to create a new account. Same shape as LoginMessage,
// different "type" - kept as a separate message deliberately: register
// and login are separate, explicit client actions (an unrecognized
// username at login is a failure, not an implicit registration).
namespace RegisterMessage
{
    std::string build(const std::string& username, const std::string& password);

    // Returns false (leaving outUsername/outPassword untouched) if this
    // isn't a valid register message.
    bool parse(const std::string& rawMessage, std::string& outUsername, std::string& outPassword);
}
