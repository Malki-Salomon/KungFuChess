// KungFuChessGUI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// This process runs NO game logic. It is a pure display-and-input client:
// it renders whatever board the server sends it, and forwards clicks as
// move/jump requests. The game engine (Core, server-side) is the sole
// source of truth for legality, ownership, turn order and game state -
// see the Level 5 directive.
#include "img.hpp"
#include "Events/EventBus.h"
#include "GameWindow.h"
#include "NetworkSnapshotAdapter.h"
#include "InputHandler.h"
#include "GameLoop.h"

#include "WebSocketClient.h"
#include "ConsoleLoginFlow.h"
#include "MoveSender.h"
#include "Logger.h"

#include <iostream>
#include <string>
#include <opencv2/highgui.hpp>

namespace
{
    constexpr unsigned short kServerPort = 9000; // matches Server.cpp's kListenPort
}

int main()
{
    try {
        Logger::init("gui-client.log");

        WebSocketClient client;
        if (!client.connect("localhost", kServerPort))
        {
            std::cerr << "Could not connect to the server. Is KungFuChessServer.exe running?\n";
            // WebSocketClient::connect() already logged the specific failure.
            return 1;
        }

        // Deliberately done in the console, BEFORE any graphics window
        // opens: OpenCV cannot do text entry, and a native input dialog is
        // significant separate work, so this reuses the already-proven
        // console flow shared with KungFuChessShellClient. Replacing it
        // with an in-window input box is a reasonable future enhancement.
        std::string username, roomCode, role;
        if (!runConsoleLoginAndRoomFlow(client, username, roomCode, role))
        {
            client.close();
            return 1;
        }

        // The sole notification channel between the network layer and the
        // display. NetworkSnapshotAdapter publishes onto it; GameWindow
        // subscribes. Neither holds a reference to the other.
        EventBus bus;

        // Uniquely identifies this session's display window. In a
        // multi-session setup this would be the session/game ID rather
        // than a fixed literal, so concurrent windows never collide on the
        // same OpenCV window name.
        const std::string windowName = "GameWindow_Session1";

        GameWindow gameWindow(windowName, bus);
        NetworkSnapshotAdapter snapshotAdapter(bus);
        MoveSender moveSender(client);
        InputHandler inputHandler(moveSender, gameWindow.getLayout(), gameWindow.getWindowName(),
                                   gameWindow.getMoveIntentHint());

        // Only now start receiving: everything a board message will touch
        // on arrival (the bus, the window, the adapter) already exists, so
        // the very first snapshot can't race construction.
        client.startReceiveLoop([&snapshotAdapter](const std::string& text)
        {
            snapshotAdapter.onMessage(text);
        });

        // The first paint happens inside GameLoop::run(), on this thread:
        // the board arriving on the receive thread only parks a snapshot
        // (see GameWindow's THREADING note), and the loop's first tick()
        // is what actually draws it and pumps OpenCV's message loop.
        GameLoop gameLoop(gameWindow);
        gameLoop.run();

        // Ends the receive thread before the objects its handler captures
        // start going out of scope.
        client.close();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
