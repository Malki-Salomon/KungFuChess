#pragma once

#include <functional>
#include <memory>
#include <string>
#include <thread>

// Proof-of-concept WebSocket listener, now extended to move text both ways:
//   - setMessageHandler(): called (on the I/O thread) with the raw text of
//     every message received from any connected client.
//   - broadcast(): sends text to every currently connected client. Safe to
//     call from any thread - internally marshals the actual write onto the
//     I/O thread.
//
// Still no protocol/session-routing here - single global broadcast, no
// per-client addressing. That's fine while there's only ever one
// GameSession; revisit once multiple concurrent games exist.
class WebSocketServer
{
public:
    using MessageHandler = std::function<void(const std::string&)>;

    explicit WebSocketServer(unsigned short port);
    ~WebSocketServer();

    WebSocketServer(const WebSocketServer&) = delete;
    WebSocketServer& operator=(const WebSocketServer&) = delete;

    void start();
    void stop();

    // Must be called before start() - the handler is captured once, not
    // safe to change while the I/O thread is running.
    void setMessageHandler(MessageHandler handler);

    // Thread-safe: safe to call from the game tick thread.
    void broadcast(const std::string& text);

private:
    struct Impl;

    unsigned short m_port;
    std::unique_ptr<Impl> m_impl;
    std::thread m_ioThread;
};
