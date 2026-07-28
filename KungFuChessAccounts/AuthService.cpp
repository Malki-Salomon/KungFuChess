#include "AuthService.h"

#include "PasswordHasher.h"

AuthService::AuthService(IUserRepository& repo)
    : m_repo(repo)
{
}

AuthResult AuthService::registerAccount(const std::string& username, const std::string& password)
{
    if (m_repo.usernameExists(username))
        return { false, "username already exists", 0 };

    std::string passwordHash;
    if (!PasswordHasher::hash(password, passwordHash))
        return { false, "could not hash password", 0 };

    if (!m_repo.createAccount(username, passwordHash))
        return { false, "could not create account", 0 };

    // Query back rather than assuming a starting value here - the
    // repository (not AuthService) owns what a fresh account's rating is.
    return { true, "", m_repo.getRating(username) };
}

AuthResult AuthService::login(const std::string& username, const std::string& password)
{
    std::string storedHash;
    if (!m_repo.getPasswordHash(username, storedHash))
        return { false, "unknown username", 0 };

    if (!PasswordHasher::verify(password, storedHash))
        return { false, "wrong password", 0 };

    return { true, "", m_repo.getRating(username) };
}
