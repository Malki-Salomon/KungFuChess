#pragma once

#include <functional>
#include <memory>
#include <string>

// Blocking WebSocket client (Boost.Beast). Simple by design for a console
// tool: connect() and send() block the calling thread; startReceiveLoop()
// spawns one background thread that blocks reading messages and hands
// each one to the given handler, until the connection closes.
//
// One thread reading while another writes on the same stream is a
// supported Beast usage pattern - that's the intended shape here (main
// thread sends user input, background thread prints incoming board
// updates). close() safely unblocks a pending read (by cancelling the
// underlying socket, which Asio documents as safe to do cross-thread)
// before touching the websocket layer itself.
class WebSocketClient
{
public:
    using MessageHandler = std::function<void(const std::string&)>;

    WebSocketClient();
    ~WebSocketClient();

    WebSocketClient(const WebSocketClient&) = delete;
    WebSocketClient& operator=(const WebSocketClient&) = delete;

    // Blocking. Returns false (with a message already printed to stderr)
    // on failure to resolve/connect/handshake.
    bool connect(const std::string& host, unsigned short port);

    // Blocking send of one text message.
    void send(const std::string& text);

    // Spawns a background thread that blocks on reads, calling handler
    // with each message's raw text, until the connection closes or
    // errors. Call once, after connect().
    void startReceiveLoop(MessageHandler handler);

    void close();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
