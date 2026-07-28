#include "WebSocketClient.h"

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <iostream>
#include <thread>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

struct WebSocketClient::Impl
{
    asio::io_context ioContext;
    websocket::stream<tcp::socket> ws{ ioContext };
    std::thread receiveThread;
};

WebSocketClient::WebSocketClient()
    : m_impl(std::make_unique<Impl>())
{
}

WebSocketClient::~WebSocketClient()
{
    close();
}

bool WebSocketClient::connect(const std::string& host, unsigned short port)
{
    try
    {
        tcp::resolver resolver(m_impl->ioContext);
        auto endpoints = resolver.resolve(host, std::to_string(port));

        auto& socket = beast::get_lowest_layer(m_impl->ws);
        socket.connect(*endpoints.begin());

        m_impl->ws.handshake(host, "/");
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[WebSocketClient] connect failed: " << e.what() << "\n";
        return false;
    }
}

void WebSocketClient::send(const std::string& text)
{
    try
    {
        m_impl->ws.text(true);
        m_impl->ws.write(asio::buffer(text));
    }
    catch (const std::exception& e)
    {
        std::cerr << "[WebSocketClient] send failed: " << e.what() << "\n";
    }
}

std::string WebSocketClient::receiveOne()
{
    try
    {
        beast::flat_buffer buffer;
        m_impl->ws.read(buffer);
        return beast::buffers_to_string(buffer.data());
    }
    catch (const std::exception& e)
    {
        std::cerr << "[WebSocketClient] receiveOne failed: " << e.what() << "\n";
        return std::string();
    }
}

void WebSocketClient::startReceiveLoop(MessageHandler handler)
{
    m_impl->receiveThread = std::thread([this, handler]
    {
        beast::flat_buffer buffer;
        while (true)
        {
            try
            {
                buffer.clear();
                m_impl->ws.read(buffer);
                std::string text = beast::buffers_to_string(buffer.data());
                if (handler)
                    handler(text);
            }
            catch (const std::exception&)
            {
                // Connection closed or errored - end the loop quietly.
                return;
            }
        }
    });
}

void WebSocketClient::close()
{
    // startReceiveLoop()'s background thread blocks in a *synchronous*
    // ws.read(), parked directly in the OS recv() call. cancel() (this
    // function's previous approach) only affects Asio's *asynchronous*
    // operation table - it does nothing to a thread blocked in a
    // synchronous read, which is why close() used to hang here waiting on
    // join() below.
    //
    // Shutting down and closing the underlying socket from this thread
    // does unblock it: on both POSIX and Windows, closing a socket that
    // another thread is blocked reading from causes that blocked call to
    // return with an error. That's also why this does NOT call
    // ws.close() (the websocket-level close handshake) concurrently with
    // the background thread's read - Beast does not guarantee that's
    // safe on the same stream, unlike this raw-socket shutdown/close.
    beast::error_code ec;
    auto& socket = beast::get_lowest_layer(m_impl->ws);
    socket.shutdown(asio::socket_base::shutdown_both, ec);
    socket.close(ec);

    if (m_impl->receiveThread.joinable())
        m_impl->receiveThread.join();
}
