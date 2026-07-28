#pragma once

#include <string>

// Storage abstraction for accounts (password hash + rating). AuthService
// depends on this, not on any concrete storage - so the concrete
// implementation can change (InMemoryUserRepository now,
// SqliteUserRepository in a later stage) without AuthService or anything
// above it changing.
class IUserRepository
{
public:
    virtual ~IUserRepository() = default;

    virtual bool usernameExists(const std::string& username) = 0;

    // Fails (returns false) if the username already exists. The new
    // account's starting rating is the implementation's concern, not the
    // caller's - see each implementation.
    virtual bool createAccount(const std::string& username, const std::string& passwordHash) = 0;

    // Returns false (leaving outHash untouched) if the username doesn't
    // exist.
    virtual bool getPasswordHash(const std::string& username, std::string& outHash) = 0;

    // Precondition: username exists. Behavior for an unknown username is
    // implementation-defined - callers are expected to have already
    // checked (e.g. via usernameExists() or a successful getPasswordHash()).
    virtual int getRating(const std::string& username) = 0;

    virtual void updateRating(const std::string& username, int newRating) = 0;
};
