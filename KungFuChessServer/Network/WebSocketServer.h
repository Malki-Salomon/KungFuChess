#pragma once

#include "SessionId.h"

#include <functional>
#include <memory>
#include <string>
#include <thread>

// WebSocket listener: accepts connections, moves text both ways, and now
// tracks each connection's identity (SessionId) so a caller can tell
// connections apart - e.g. to assign one player white and another black.
//   - setConnectHandler(): called once a connection's handshake succeeds.
//   - setMessageHandler(): called with (sender's SessionId, message text)
//     for every message received from any connected client.
//   - setDisconnectHandler(): called once a connection is gone.
//   - broadcast(): sends text to every currently connected client.
//   - sendTo(): sends text to exactly one connection by SessionId.
// broadcast()/sendTo() are safe to call from any thread - they marshal the
// actual write onto the I/O thread internally. The three handlers all fire
// ON the I/O thread - a caller that needs to touch anything not safe for
// concurrent access (like Core/Game) must hand off through something like
// CommandInbox rather than acting directly inside a handler.
class WebSocketServer
{
public:
    using MessageHandler = std::function<void(SessionId, const std::string&)>;
    using ConnectHandler = std::function<void(SessionId)>;
    using DisconnectHandler = std::function<void(SessionId)>;

    explicit WebSocketServer(unsigned short port);
    ~WebSocketServer();

    WebSocketServer(const WebSocketServer&) = delete;
    WebSocketServer& operator=(const WebSocketServer&) = delete;

    void start();
    void stop();

    // All three handlers must be set before start() - captured once, not
    // safe to change while the I/O thread is running.
    void setConnectHandler(ConnectHandler handler);
    void setMessageHandler(MessageHandler handler);
    void setDisconnectHandler(DisconnectHandler handler);

    // Thread-safe: safe to call from the game tick thread.
    void broadcast(const std::string& text);

    // Thread-safe. No-op if the session no longer exists (already
    // disconnected) - callers don't need to check first.
    void sendTo(SessionId id, const std::string& text);

private:
    struct Impl;

    unsigned short m_port;
    std::unique_ptr<Impl> m_impl;
    std::thread m_ioThread;
};
