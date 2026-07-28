#include "doctest.h"
#include "AuthService.h"
#include "InMemoryUserRepository.h"

TEST_SUITE("AuthService")
{
    TEST_CASE("Register succeeds for a new username")
    {
        InMemoryUserRepository repo;
        AuthService auth(repo);

        AuthResult result = auth.registerAccount("alice", "hunter2");

        CHECK(result.success);
        CHECK(result.rating == 1200);
    }

    TEST_CASE("Register fails for a username that already exists")
    {
        InMemoryUserRepository repo;
        AuthService auth(repo);

        auth.registerAccount("alice", "hunter2");
        AuthResult result = auth.registerAccount("alice", "a-different-password");

        CHECK_FALSE(result.success);
    }

    TEST_CASE("Login succeeds with the correct password")
    {
        InMemoryUserRepository repo;
        AuthService auth(repo);

        auth.registerAccount("alice", "hunter2");
        AuthResult result = auth.login("alice", "hunter2");

        CHECK(result.success);
        CHECK(result.rating == 1200);
    }

    TEST_CASE("Login fails with the wrong password")
    {
        InMemoryUserRepository repo;
        AuthService auth(repo);

        auth.registerAccount("alice", "hunter2");
        AuthResult result = auth.login("alice", "wrong-password");

        CHECK_FALSE(result.success);
    }

    TEST_CASE("Login fails for an unknown username")
    {
        InMemoryUserRepository repo;
        AuthService auth(repo);

        AuthResult result = auth.login("nobody", "whatever");

        CHECK_FALSE(result.success);
    }

    TEST_CASE("A second login on an already-logged-in identity is treated as a fresh attempt")
    {
        InMemoryUserRepository repo;
        AuthService auth(repo);

        auth.registerAccount("alice", "hunter2");
        AuthResult first = auth.login("alice", "hunter2");
        AuthResult second = auth.login("alice", "hunter2");

        CHECK(first.success);
        CHECK(second.success);
        CHECK(second.rating == first.rating);
    }
}
