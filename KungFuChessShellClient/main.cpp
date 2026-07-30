#include "WebSocketClient.h"
#include "ConsoleLoginFlow.h"
#include "MoveSender.h"

#include "BoardMessage.h"
#include "GameOverMessage.h"
#include "Logger.h"

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
    Logger::init("client.log");

    WebSocketClient client;
    if (!client.connect("localhost", kServerPort))
    {
        std::cerr << "Could not connect to the server. Is KungFuChessServer.exe running?\n";
        // WebSocketClient::connect() already logged the specific failure.
        return 1;
    }

    std::string username;
    std::string roomCode;
    std::string role;
    if (!runConsoleLoginAndRoomFlow(client, username, roomCode, role))
    {
        client.close();
        return 1;
    }

    client.startReceiveLoop([](const std::string& text)
    {
        std::vector<std::vector<std::string>> rows;
        if (BoardMessage::parse(text, rows))
        {
            // Deliberately not logged - board updates arrive frequently
            // during an active game, and logging every one would make
            // the log file noisy without adding real debugging value.
            printBoard(rows);
            return;
        }

        std::string result;
        bool hasRatings = false;
        int whiteNewRating = 0;
        int blackNewRating = 0;
        if (GameOverMessage::parse(text, result, hasRatings, whiteNewRating, blackNewRating))
        {
            Logger::info("[Client] game over: " + result + " wins" +
                (hasRatings
                    ? (" (white -> " + std::to_string(whiteNewRating) + ", black -> " + std::to_string(blackNewRating) + ")")
                    : std::string(" (no rating update)")));
            return;
        }

        // Anything else (e.g. "assigned") is silently ignored here for
        // now - this client only acts on board updates and game-over.
    });

    std::cout << "Logged in as \"" << username << "\".\n";
    std::cout << "Enter moves as two squares, e.g.: e2 e4\n";
    std::cout << "Type 'quit' to exit.\n> " << std::flush;

    MoveSender moveSender(client);

    std::string from;
    while (std::cin >> from)
    {
        if (from == "quit")
            break;

        std::string to;
        if (!(std::cin >> to))
            break;

        moveSender.sendMove(from, to);
        std::cout << "> " << std::flush;
    }

    client.close();
    return 0;
}
