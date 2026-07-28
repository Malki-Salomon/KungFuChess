#pragma once

#include <string>

// {"type":"authResult","success":true,"message":"","rating":1200} - the
// server's reply to both a "register" and a "login" message (one shape
// for both, since from the client's point of view they're the same
// question: "did that succeed, and what's my rating now?"). On failure,
// "message" carries a short reason (e.g. "wrong password") and "rating"
// is 0.
namespace AuthResultMessage
{
    std::string build(bool success, const std::string& message, int rating);

    // Returns false (leaving the out-params untouched) if this isn't a
    // valid authResult message.
    bool parse(const std::string& rawMessage, bool& outSuccess, std::string& outMessage, int& outRating);
}
