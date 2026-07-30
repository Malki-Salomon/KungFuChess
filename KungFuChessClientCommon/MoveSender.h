#pragma once

#include <string>

class WebSocketClient;

// The one place a client builds and sends the outgoing game actions
// (MoveMessage, JumpMessage). Keeps WebSocketClient protocol-agnostic (it
// only ever moves plain strings) and keeps callers like InputHandler free
// of any wire-format knowledge - they just name algebraic squares
// ("e2", "e4").
class MoveSender
{
public:
    explicit MoveSender(WebSocketClient& client);

    void sendMove(const std::string& from, const std::string& to);

    // A jump names one square only - the piece there goes briefly airborne
    // and lands back where it started, so there is no destination to give.
    void sendJump(const std::string& square);

private:
    WebSocketClient& client;
};
