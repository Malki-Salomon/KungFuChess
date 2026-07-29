#pragma once

#include "SessionId.h"

#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>

// WebSocket listener: accepts connections, moves text both ways, and now
// tracks each connection's identity (SessionId) so a caller can tell
// connections apart - e.g. to assign one player white and another black.
//   - setConnectHandler(): called once a connection's handshake succeeds.
//   - setMessageHandler(): called with (sender's SessionId, message text)
//     for every message received from any connected client.
//   - setDisconnectHandler(): called once a connection is gone.
//   - sendTo(): sends text to exactly one connection by SessionId.
//   - sendToMany(): sends text to a specific list of connections (e.g. one
//     room's members) - just sendTo() in a loop, but centralized so
//     callers don't each reimplement the loop.
// sendTo()/sendToMany() are safe to call from any thread - they marshal the
// actual write onto the I/O thread internally. The three handlers all fire
// ON the I/O thread - a caller that needs to touch anything not safe for
// concurrent access (like Core/Game) must hand off through something like
// CommandInbox rather than acting directly inside a handler.
//
// Deliberately has no notion of "broadcast to everyone" or "who's logged
// in" - every message in this protocol now targets either one connection
// or one room's member list (computed by Room/RoomManager, which do know
// about logins and room membership); WebSocketServer itself stays exactly
// as Application-agnostic as ever.
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

    // Thread-safe. No-op if the session no longer exists (already
    // disconnected) - callers don't need to check first.
    void sendTo(SessionId id, const std::string& text);

    // Thread-safe. Just calls sendTo() for each id - see the class
    // comment for why the recipient list is always computed by the
    // caller (Room/RoomManager), never by WebSocketServer itself.
    void sendToMany(const std::vector<SessionId>& ids, const std::string& text);

private:
    struct Impl;

    unsigned short m_port;
    std::unique_ptr<Impl> m_impl;
    std::thread m_ioThread;
};
