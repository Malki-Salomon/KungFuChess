#include "doctest.h"
#include "PlayerDirectory.h"

TEST_SUITE("PlayerDirectory")
{
    TEST_CASE("isLoggedIn is false before set()")
    {
        PlayerDirectory directory;

        CHECK_FALSE(directory.isLoggedIn(1));
    }

    TEST_CASE("isLoggedIn is true after set()")
    {
        PlayerDirectory directory;

        directory.set(1, "alice");

        CHECK(directory.isLoggedIn(1));
    }

    TEST_CASE("isLoggedIn is false again after release()")
    {
        PlayerDirectory directory;

        directory.set(1, "alice");
        directory.release(1);

        CHECK_FALSE(directory.isLoggedIn(1));
    }
}
