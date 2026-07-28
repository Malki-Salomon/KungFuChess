#include "WebSocketClient.h"

#include "LoginMessage.h"
#include "MoveMessage.h"
#include "BoardMessage.h"

#include <iostream>
#include <mutex>
#include <string>
#include <vector>

namespace
{
    std::mutex g_consoleMutex; // just to keep the receive thread's board
                                // prints from interleaving mid-line with
                                // the main thread's prompts

    void printBoard(const std::vector<std::vector<std::string>>& rows)
    {
        std::lock_guard<std::mutex> lock(g_consoleMutex);
        std::cout << "\n";
        for (const auto& row : rows)
        {
            for (const auto& cell : row)
                std::cout << cell << " ";
            std::cout << "\n";
        }
        std::cout << "\n> " << std::flush;
    }

    constexpr unsigned short kServerPort = 9000; // matches Server.cpp's kListenPort
}

int main()
{
    std::cout << "Enter username: ";
    std::string username;
    std::getline(std::cin, username);

    WebSocketClient client;
    if (!client.connect("localhost", kServerPort))
    {
        std::cerr << "Could not connect to the server. Is KungFuChessServer.exe running?\n";
        return 1;
    }

    client.send(LoginMessage::build(username));

    client.startReceiveLoop([](const std::string& text)
    {
        std::vector<std::vector<std::string>> rows;
        if (BoardMessage::parse(text, rows))
        {
            printBoard(rows);
        }
        // Non-board messages (e.g. a future "assigned color" message) are
        // silently ignored here for now - this client only understands
        // board updates at this stage.
    });

    std::cout << "Logged in as \"" << username << "\".\n";
    std::cout << "Enter moves as two squares, e.g.: e2 e4\n";
    std::cout << "Type 'quit' to exit.\n> " << std::flush;

    std::string from;
    while (std::cin >> from)
    {
        if (from == "quit")
            break;

        std::string to;
        if (!(std::cin >> to))
            break;

        client.send(MoveMessage::build(from, to));
        std::cout << "> " << std::flush;
    }

    client.close();
    return 0;
}
