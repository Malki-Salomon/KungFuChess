#pragma once

#include <string>

// Real password hashing via libsodium's crypto_pwhash_str()/
// crypto_pwhash_str_verify() (Argon2id under the hood). Salt and
// parameters are embedded in the output string itself, so no separate
// salt management is needed. Uses the _INTERACTIVE presets - libsodium's
// own recommended default for this kind of use case, as opposed to the
// intentionally slower/heavier _MODERATE/_SENSITIVE presets meant for
// different scenarios.
//
// This is the ONLY file in the entire codebase that includes sodium.h or
// calls any libsodium function - not even this header leaks it.
namespace PasswordHasher
{
    // Must be called exactly once, before any hash()/verify() call -
    // libsodium requires its own one-time global initialization. Safe to
    // call more than once (subsequent calls are no-ops that still return
    // true). Returns false if initialization failed, in which case
    // hash()/verify() are not safe to use.
    bool init();

    bool hash(const std::string& plainPassword, std::string& outHash);
    bool verify(const std::string& plainPassword, const std::string& storedHash);
}
