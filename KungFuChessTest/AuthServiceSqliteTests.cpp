#include "doctest.h"
#include "AuthService.h"
#include "SqliteUserRepository.h"

#include <cstdio>
#include <string>

namespace
{
    // Same test cases as AuthServiceTests.cpp, against SqliteUserRepository
    // instead of InMemoryUserRepository - proving the two are interchangeable
    // from AuthService's point of view, per the IUserRepository contract.
    // Each test gets its own fresh temp .db file (wiped before use so a
    // leftover file from a previously-failed run can't affect the result).
    std::string freshTempDbPath(const std::string& name)
    {
        std::string path = "test_" + name + ".db";
        std::remove(path.c_str());
        return path;
    }
}

TEST_SUITE("AuthService (SqliteUserRepository)")
{
    TEST_CASE("Register succeeds for a new username")
    {
        std::string dbPath = freshTempDbPath("register_succeeds");

        {
            // Scoped so the sqlite3 connection is closed (repo destroyed)
            // before std::remove() below - on Windows an open file can't
            // be deleted, so this ordering isn't optional.
            SqliteUserRepository repo(dbPath);
            AuthService auth(repo);

            AuthResult result = auth.registerAccount("alice", "hunter2");

            CHECK(result.success);
            CHECK(result.rating == 1200);
        }

        std::remove(dbPath.c_str());
    }

    TEST_CASE("Register fails for a username that already exists")
    {
        std::string dbPath = freshTempDbPath("register_duplicate");

        {
            SqliteUserRepository repo(dbPath);
            AuthService auth(repo);

            auth.registerAccount("alice", "hunter2");
            AuthResult result = auth.registerAccount("alice", "a-different-password");

            CHECK_FALSE(result.success);
        }

        std::remove(dbPath.c_str());
    }

    TEST_CASE("Login succeeds with the correct password")
    {
        std::string dbPath = freshTempDbPath("login_succeeds");

        {
            SqliteUserRepository repo(dbPath);
            AuthService auth(repo);

            auth.registerAccount("alice", "hunter2");
            AuthResult result = auth.login("alice", "hunter2");

            CHECK(result.success);
            CHECK(result.rating == 1200);
        }

        std::remove(dbPath.c_str());
    }

    TEST_CASE("Login fails with the wrong password")
    {
        std::string dbPath = freshTempDbPath("login_wrong_password");

        {
            SqliteUserRepository repo(dbPath);
            AuthService auth(repo);

            auth.registerAccount("alice", "hunter2");
            AuthResult result = auth.login("alice", "wrong-password");

            CHECK_FALSE(result.success);
        }

        std::remove(dbPath.c_str());
    }

    TEST_CASE("Login fails for an unknown username")
    {
        std::string dbPath = freshTempDbPath("login_unknown_user");

        {
            SqliteUserRepository repo(dbPath);
            AuthService auth(repo);

            AuthResult result = auth.login("nobody", "whatever");

            CHECK_FALSE(result.success);
        }

        std::remove(dbPath.c_str());
    }

    TEST_CASE("A second login on an already-logged-in identity is treated as a fresh attempt")
    {
        std::string dbPath = freshTempDbPath("second_login_fresh_attempt");

        {
            SqliteUserRepository repo(dbPath);
            AuthService auth(repo);

            auth.registerAccount("alice", "hunter2");
            AuthResult first = auth.login("alice", "hunter2");
            AuthResult second = auth.login("alice", "hunter2");

            CHECK(first.success);
            CHECK(second.success);
            CHECK(second.rating == first.rating);
        }

        std::remove(dbPath.c_str());
    }

    TEST_CASE("Durability: data survives independently of any single repository instance's lifetime")
    {
        std::string dbPath = freshTempDbPath("durability");

        {
            SqliteUserRepository repo(dbPath);
            AuthService auth(repo);

            AuthResult registerResult = auth.registerAccount("alice", "hunter2");
            REQUIRE(registerResult.success);
        } // repo (and its sqlite3 connection) is destroyed here

        {
            // Brand new instance, same file - proves the data was actually
            // written to disk, not just cached in the first instance.
            SqliteUserRepository freshRepo(dbPath);
            AuthService freshAuth(freshRepo);

            AuthResult loginResult = freshAuth.login("alice", "hunter2");

            CHECK(loginResult.success);
            CHECK(loginResult.rating == 1200);
        }

        std::remove(dbPath.c_str());
    }
}
