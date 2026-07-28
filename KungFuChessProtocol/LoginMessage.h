#pragma once

#include <string>

// {"type":"login","username":"...","password":"..."} - sent by a client
// that already has an account, to authenticate. Fails (see
// AuthResultMessage) if the username is unrecognized or the password is
// wrong - login never auto-creates an account (see RegisterMessage for
// that).
namespace LoginMessage
{
    std::string build(const std::string& username, const std::string& password);

    // Returns false (leaving outUsername/outPassword untouched) if this
    // isn't a valid login message.
    bool parse(const std::string& rawMessage, std::string& outUsername, std::string& outPassword);
}
