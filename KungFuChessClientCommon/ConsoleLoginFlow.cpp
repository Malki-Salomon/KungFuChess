#include "ConsoleLoginFlow.h"

#include "WebSocketClient.h"

#include "LoginMessage.h"
#include "RegisterMessage.h"
#include "AuthResultMessage.h"
#include "CreateRoomMessage.h"
#include "JoinRoomMessage.h"
#include "RoomCreatedMessage.h"
#include "RoomJoinedMessage.h"
#include "RoomJoinFailedMessage.h"
#include "Logger.h"

#include <iostream>

namespace
{
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
                Logger::error("[Client] connection closed while waiting for an authResult reply");
                return false;
            }

            bool success = false;
            std::string message;
            int rating = 0;
            if (!AuthResultMessage::parse(raw, success, message, rating))
                continue;

            if (success)
            {
                std::cout << "Success. Rating: " << rating << "\n";
                Logger::info("[Client] auth succeeded, rating " + std::to_string(rating));
            }
            else
            {
                std::cout << "Failed: " << message << "\n";
                Logger::warn("[Client] auth failed: " + message);
            }

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
                Logger::error("[Client] connection closed while waiting for a roomCreated reply");
                return false;
            }

            if (RoomCreatedMessage::parse(raw, outCode))
            {
                Logger::info("[Client] room " + outCode + " created");
                return true;
            }
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
                Logger::error("[Client] connection closed while waiting for a roomJoined reply");
                return false;
            }

            std::string code;
            if (RoomJoinedMessage::parse(raw, code, outRole))
            {
                Logger::info("[Client] joined room " + code + " as " + outRole);
                return true;
            }

            std::string reason;
            if (RoomJoinFailedMessage::parse(raw, reason))
            {
                std::cerr << "Failed to join room: " << reason << "\n";
                Logger::warn("[Client] failed to join room: " + reason);
                return false;
            }
        }
    }
}

bool runConsoleLoginAndRoomFlow(WebSocketClient& client,
                                 std::string& outUsername,
                                 std::string& outRoomCode,
                                 std::string& outRole)
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

    if (isRegister)
    {
        client.send(RegisterMessage::build(username, password));
        if (!awaitAuthResult(client))
            return false;
        std::cout << "Account created. Logging in...\n";
    }

    // Register and login are separate, explicit messages even when chained
    // here back-to-back - the server never treats a successful register as
    // an implicit login.
    client.send(LoginMessage::build(username, password));
    if (!awaitAuthResult(client))
        return false;

    outUsername = username;

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
            return false;
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
        return false;
    std::cout << "Joined room \"" << roomCode << "\" as " << role << ".\n";

    outRoomCode = roomCode;
    outRole = role;
    return true;
}
