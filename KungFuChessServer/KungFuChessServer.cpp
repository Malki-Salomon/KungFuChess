// KungFuChessServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Application/Server.h"
#include "PasswordHasher.h"

#include <cstdlib>
#include <iostream>

int main()
{
    // Must happen before any AuthService register/login call, which is
    // the only thing that actually invokes PasswordHasher::hash()/verify()
    // - doing it here, before Server (and therefore the network listener)
    // even exists, guarantees that ordering.
    if (!PasswordHasher::init())
    {
        std::cerr << "Failed to initialize libsodium - cannot continue.\n";
        return EXIT_FAILURE;
    }

    try
    {
        Server server;
        server.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
