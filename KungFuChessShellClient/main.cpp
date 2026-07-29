#include "WebSocketClient.h"

#include "LoginMessage.h"
#include "RegisterMessage.h"
#include "AuthResultMessage.h"
#include "MoveMessage.h"
#include "BoardMessage.h"
#include "CreateRoomMessage.h"
#include "JoinRoomMessage.h"
#include "RoomCreatedMessage.h"
#include "RoomJoinedMessage.h"
#include "RoomJoinFailedMessage.h"

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

    // Blocking: waits for the server's authResult reply to a register/login
    // just sent, prints the outcome, and returns whether it succeeded.
    // Other messages may legitimately arrive first - the server broadcasts
    // board snapshots to every connected socket, including ones that
    // haven't logged in yet - so this discards anything that isn't the
    // authResult it's actually waiting for.
    bool awaitAuthResult(WebSocketClient& client)
    {
        while (true)
        {
            std::string raw = client.receiveOne();
            if (raw.empty())
            {
                std::cerr << "Connection closed while waiting for a reply from the server.\n";
                return false;
            }

            bool success = false;
            std::string message;
            int rating = 0;
            if (!AuthResultMessage::parse(raw, success, message, rating))
                continue;

            if (success)
                std::cout << "Success. Rating: " << rating << "\n";
            else
                std::cout << "Failed: " << message << "\n";

            return success;
        }
    }

    // Blocking: waits for the server's roomCreated reply to a createRoom
    // just sent, same "discard anything else" pattern as awaitAuthResult.
    bool awaitRoomCreated(WebSocketClient& client, std::string& outCode)
    {
        while (true)
        {
            std::string raw = client.receiveOne();
            if (raw.empty())
            {
                std::cerr << "Connection closed while waiting for a reply from the server.\n";
                return false;
            }

            if (RoomCreatedMessage::parse(raw, outCode))
                return true;
        }
    }

    // Blocking: waits for the server's roomJoined/roomJoinFailed reply to
    // a joinRoom just sent.
    bool awaitRoomJoined(WebSocketClient& client, std::string& outRole)
    {
        while (true)
        {
            std::string raw = client.receiveOne();
            if (raw.empty())
            {
                std::cerr << "Connection closed while waiting for a reply from the server.\n";
                return false;
            }

            std::string code;
            if (RoomJoinedMessage::parse(raw, code, outRole))
                return true;

            std::string reason;
            if (RoomJoinFailedMessage::parse(raw, reason))
            {
                std::cerr << "Failed to join room: " << reason << "\n";
                return false;
            }
        }
    }
}

int main()
{
    std::cout << "Register a new account or log in to an existing one? [r/l]: ";
    std::string choice;
    std::getline(std::cin, choice);
    bool isRegister = !choice.empty() && (choice[0] == 'r' || choice[0] == 'R');

    std::cout << "Username: ";
    std::string username;
    std::getline(std::cin, username);

    std::cout << "Password: ";
    std::string password;
    std::getline(std::cin, password);

    WebSocketClient client;
    if (!client.connect("localhost", kServerPort))
    {
        std::cerr << "Could not connect to the server. Is KungFuChessServer.exe running?\n";
        return 1;
    }

    if (isRegister)
    {
        client.send(RegisterMessage::build(username, password));
        if (!awaitAuthResult(client))
        {
            client.close();
            return 1;
        }
        std::cout << "Account created. Logging in...\n";
    }

    // Register and login are separate, explicit messages even when chained
    // here back-to-back - the server never treats a successful register as
    // an implicit login.
    client.send(LoginMessage::build(username, password));
    if (!awaitAuthResult(client))
    {
        client.close();
        return 1;
    }

    // Nothing happens for this connection until it also creates or joins
    // a room - there's no longer an implicit single global game to fall
    // into (see the room-creation stage). Mirrors the register-vs-login
    // prompt style above.
    std::cout << "Create a new room or join one by code? [c/j]: ";
    std::string roomChoice;
    std::getline(std::cin, roomChoice);
    bool isCreateRoom = !roomChoice.empty() && (roomChoice[0] == 'c' || roomChoice[0] == 'C');

    std::string roomCode;
    if (isCreateRoom)
    {
        client.send(CreateRoomMessage::build());
        if (!awaitRoomCreated(client, roomCode))
        {
            client.close();
            return 1;
        }
        std::cout << "Room created: " << roomCode << ". Joining it...\n";
    }
    else
    {
        std::cout << "Room code: ";
        std::getline(std::cin, roomCode);
    }

    // Creating a room does not, by itself, seat the creator in it - a
    // separate, explicit joinRoom is always required, even right after
    // creating.
    client.send(JoinRoomMessage::build(roomCode));
    std::string role;
    if (!awaitRoomJoined(client, role))
    {
        client.close();
        return 1;
    }
    std::cout << "Joined room \"" << roomCode << "\" as " << role << ".\n";

    client.startReceiveLoop([](const std::string& text)
    {
        std::vector<std::vector<std::string>> rows;
        if (BoardMessage::parse(text, rows))
        {
            printBoard(rows);
        }
        // Non-board messages (e.g. "assigned" or "gameOver") are silently
        // ignored here for now - this client only understands board
        // updates at this stage.
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
