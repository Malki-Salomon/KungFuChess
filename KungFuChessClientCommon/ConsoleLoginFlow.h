#pragma once

#include <string>

class WebSocketClient;

// Console-driven register/login + create/join-room flow shared by every
// text-console-capable client (KungFuChessShellClient, and KungFuChessGUI's
// startup - see the GUI directive's design decision for why a graphics
// window still goes through this console flow for text entry). Prompts on
// stdin/stdout and blocks on WebSocketClient replies exactly the way
// KungFuChessShellClient's main() used to do inline, before this was
// extracted so a second client wouldn't need its own copy.
//
// client must already be connect()'d; this function does not connect() or
// close() it. Returns false (with a message already printed to stderr) the
// moment any step fails - register/login/createRoom/joinRoom - leaving the
// out-params in whatever partial state they'd reached.
bool runConsoleLoginAndRoomFlow(WebSocketClient& client,
                                 std::string& outUsername,
                                 std::string& outRoomCode,
                                 std::string& outRole);
