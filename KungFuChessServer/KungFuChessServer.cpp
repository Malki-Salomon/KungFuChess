// KungFuChessServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Application/Server.h"
#include "PasswordHasher.h"
#include "Logger.h"

#include <cstdlib>

int main()
{
    // Absolute first thing - so any failure in anything below (including
    // PasswordHasher::init() itself) is still captured in server.log.
    Logger::init("server.log");

    // Must happen before any AuthService register/login call, which is
    // the only thing that actually invokes PasswordHasher::hash()/verify()
    // - doing it here, before Server (and therefore the network listener)
    // even exists, guarantees that ordering.
    if (!PasswordHasher::init())
    {
        Logger::error("Failed to initialize libsodium - cannot continue.");
        return EXIT_FAILURE;
    }

    try
    {
        Server server;
        server.run();
    }
    catch (const std::exception& e)
    {
        Logger::error(std::string("Unhandled exception: ") + e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
