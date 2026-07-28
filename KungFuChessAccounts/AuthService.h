#pragma once

#include "IUserRepository.h"

#include <string>

struct AuthResult
{
    bool success;
    std::string message; // empty on success
    int rating;           // 0 on failure
};

// Verifies identity (register/login) against an IUserRepository. This is
// the ONE place that decides what makes a registration or login succeed
// or fail - Server.cpp just calls in and relays the result.
//
// Deliberately knows nothing about display names (PlayerDirectory, which
// the caller populates separately once login succeeds) or seats/colors
// (PlayerAssignment) - both are unrelated facts about a connection, not
// this class's concern.
//
// Stateless across calls - it holds no per-connection/session state, so a
// connection sending login (or register) more than once is handled
// automatically as a fresh, independent attempt each time; there is no
// notion of "already logged in" here for that reason.
class AuthService
{
public:
    explicit AuthService(IUserRepository& repo);

    AuthResult registerAccount(const std::string& username, const std::string& password);
    AuthResult login(const std::string& username, const std::string& password);

private:
    IUserRepository& m_repo;
};
