#pragma once

#include <string>

// TEMPORARY - NOT CRYPTOGRAPHICALLY SECURE. Replaced with real
// libsodium/Argon2id hashing in a later stage. Exists only so AuthService's
// logic can be built and tested now without waiting on that dependency.
// Do not use this to protect real credentials, and do not let stored
// hashes from this implementation survive into a build that has the real
// one - they are not compatible.
//
// This is the real, final interface (see AuthService) - only the body
// changes when the real hashing lands.
namespace PasswordHasher
{
    bool hash(const std::string& plainPassword, std::string& outHash);
    bool verify(const std::string& plainPassword, const std::string& storedHash);
}
