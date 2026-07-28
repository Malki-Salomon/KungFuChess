#include "PasswordHasher.h"

#include <sodium.h>

namespace PasswordHasher
{
    bool init()
    {
        // sodium_init() returns 0 on the first successful call, 1 if the
        // library was already initialized (also success), -1 on failure.
        return sodium_init() >= 0;
    }

    bool hash(const std::string& plainPassword, std::string& outHash)
    {
        char buf[crypto_pwhash_STRBYTES];

        // crypto_pwhash_str() embeds a fresh random salt (and the
        // op/mem-limit parameters used) directly into the output string,
        // so two calls with the same password produce different output
        // strings, both of which still verify() successfully.
        if (crypto_pwhash_str(buf, plainPassword.c_str(), plainPassword.size(),
                               crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
        {
            return false; // out of memory - the only documented failure mode at the INTERACTIVE preset
        }

        outHash.assign(buf);
        return true;
    }

    bool verify(const std::string& plainPassword, const std::string& storedHash)
    {
        return crypto_pwhash_str_verify(storedHash.c_str(), plainPassword.c_str(), plainPassword.size()) == 0;
    }
}
