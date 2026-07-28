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
    // Unblock the receive thread's pending read (if any) first. Calling
    // cancel() on the underlying socket from a different thread than the
    // one blocked in read() is one of the few operations Boost.Asio
    // documents as safe to do concurrently - unlike calling websocket-level
    // operations (like close()) while a read is in flight on the same
    // stream, which is not guaranteed safe.
    beast::error_code ec;
    beast::get_lowest_layer(m_impl->ws).cancel(ec);

    if (m_impl->receiveThread.joinable())
        m_impl->receiveThread.join();

    try
    {
        if (m_impl->ws.is_open())
            m_impl->ws.close(websocket::close_code::normal);
    }
    catch (const std::exception&)
    {
        // Already closed/errored - nothing more to do.
    }
}
