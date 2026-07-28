#include "doctest.h"
#include "PasswordHasher.h"

// Validates the placeholder is internally consistent (same password ->
// same hash -> verify() agrees) - NOT that it's secure. It isn't; see
// PasswordHasher.h.
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
}
