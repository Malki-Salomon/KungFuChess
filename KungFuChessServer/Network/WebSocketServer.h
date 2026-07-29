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

    // Consulted by broadcast() (only broadcast() - sendTo() is always a
    // deliberate, explicit reply to one specific connection, e.g. an
    // authResult or the login catch-up snapshot, and must never be
    // gated). Returning false for a session means it is skipped for that
    // broadcast, as if it weren't connected at all. Left unset, every
    // connected session receives every broadcast (today's behavior).
    using AuthenticationCheck = std::function<bool(SessionId)>;

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

    // Same "set before start()" precondition as the three handlers above.
    // This is the ONE place broadcast()'s recipient list gets filtered -
    // see the typedef comment above for why that's deliberate.
    void setAuthenticationCheck(AuthenticationCheck check);

    // Thread-safe: safe to call from the game tick thread. Skips any
    // session the authentication check (if set) rejects.
    void broadcast(const std::string& text);

    // Thread-safe. No-op if the session no longer exists (already
    // disconnected) - callers don't need to check first. Never gated by
    // the authentication check - see its comment above.
    void sendTo(SessionId id, const std::string& text);

    // The text of the most recent broadcast() call, or an empty string if
    // nothing has been broadcast yet. Never gated by the authentication
    // check - it's a plain on-demand read, same precedent as
    // GameSession::getSnapshot() reading Core directly rather than
    // reacting to a push.
    std::string getLastSnapshot() const;

private:
    struct Impl;

    unsigned short m_port;
    std::unique_ptr<Impl> m_impl;
    std::thread m_ioThread;
};
