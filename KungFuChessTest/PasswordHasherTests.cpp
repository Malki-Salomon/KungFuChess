#include "doctest.h"
#include "PasswordHasher.h"

// Real libsodium (Argon2id) hashing as of stage 4c - see PasswordHasher.h.
// These are the same round-trip cases as stage 4a's placeholder tests
// (same password -> verify() true; wrong password -> verify() false),
// which should behave identically from the caller's perspective, plus a
// dedicated test for a property the placeholder didn't need to have: a
// fresh random salt embedded per call.
TEST_SUITE("PasswordHasher")
{
    TEST_CASE("Correct password verifies against its own hash")
    {
        std::string storedHash;
        REQUIRE(PasswordHasher::hash("correct-horse-battery-staple", storedHash));

        CHECK(PasswordHasher::verify("correct-horse-battery-staple", storedHash));
    }

    TEST_CASE("Wrong password fails verification")
    {
        std::string storedHash;
        REQUIRE(PasswordHasher::hash("correct-horse-battery-staple", storedHash));

        CHECK_FALSE(PasswordHasher::verify("wrong-password", storedHash));
    }

    TEST_CASE("Different passwords hash differently")
    {
        std::string hashA, hashB;
        REQUIRE(PasswordHasher::hash("password-a", hashA));
        REQUIRE(PasswordHasher::hash("password-b", hashB));

        CHECK(hashA != hashB);
    }

    TEST_CASE("Same password hashed twice produces two different strings, both of which verify")
    {
        std::string firstHash, secondHash;
        REQUIRE(PasswordHasher::hash("correct-horse-battery-staple", firstHash));
        REQUIRE(PasswordHasher::hash("correct-horse-battery-staple", secondHash));

        // A fresh random salt is embedded on every call, so hashing the
        // same password twice must not produce the same stored string...
        CHECK(firstHash != secondHash);

        // ...yet both must still verify successfully against the
        // original password.
        CHECK(PasswordHasher::verify("correct-horse-battery-staple", firstHash));
        CHECK(PasswordHasher::verify("correct-horse-battery-staple", secondHash));
    }
}
