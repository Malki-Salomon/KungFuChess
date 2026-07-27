// KungFuChessServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Application/Server.h"

#include <cstdlib>
#include <iostream>

int main()
{
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
