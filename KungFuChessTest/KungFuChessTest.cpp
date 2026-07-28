// KungFuChessTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include <iostream>

// Custom main (DOCTEST_CONFIG_IMPLEMENT, not _WITH_MAIN) so libsodium can
// be initialized once before any test runs - PasswordHasherTests.cpp and
// the AuthService test files all exercise real hashing now (stage 4c).
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

#include "PasswordHasher.h"

int main(int argc, char** argv)
{
    if (!PasswordHasher::init())
    {
        std::cerr << "Failed to initialize libsodium - cannot run tests.\n";
        return 1;
    }

    doctest::Context context(argc, argv);
    return context.run();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
