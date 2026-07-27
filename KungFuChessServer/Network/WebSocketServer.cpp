#include "WebSocketServer.h"

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <algorithm>
#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

namespace
{
    // Remembers the most recent broadcast text so a client that connects
    // *after* a broadcast already happened (the common case - the initial
    // board is broadcast once, immediately, likely before anyone is
    // connected yet) can still be caught up on connect instead of only
    // seeing whatever changes happen after it joins.
    class LastSnapshotStore
    {
    public:
        void set(const std::string& text)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_text = text;
        }

        std::string get()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_text;
        }

    private:
        std::mutex m_mutex;
        std::string m_text;
    };

    // One accepted connection. Does the WS handshake, then reads text
    // messages (handed to WebSocketServer's MessageHandler) and can be told
    // to send text out (queued so only one async_write is ever in flight,
    // as Beast requires).
    //
    // The whole io_context this runs on has concurrency hint 1 and only
    // ever one thread calls run() on it (see WebSocketServer::start()), so
    // every handler below - reads, writes, and posted broadcasts alike -
    // executes serially on that single thread. No additional locking is
    // needed inside Session itself for that reason.
    class Session : public std::enable_shared_from_this<Session>
    {
    public:
        Session(tcp::socket socket, WebSocketServer::MessageHandler messageHandler, LastSnapshotStore& lastSnapshotStore)
            : m_ws(std::move(socket))
            , m_messageHandler(std::move(messageHandler))
            , m_lastSnapshotStore(lastSnapshotStore)
        {
        }

        void run()
        {
            m_ws.async_accept(
                [self = shared_from_this()](beast::error_code ec)
                {
                    if (ec)
                    {
                        std::cerr << "[WebSocketServer] handshake failed: " << ec.message() << "\n";
                        return;
                    }
                    std::cout << "[WebSocketServer] client connected\n";

                    // Catch this client up on whatever the board already
                    // looks like - it joined after the last broadcast, so
                    // without this it would see nothing until the next
                    // change happens.
                    std::string current = self->m_lastSnapshotStore.get();
                    if (!current.empty())
                        self->enqueueSend(current);

                    self->readLoop();
                });
        }

        // Queues text for sending; safe to call from the I/O thread only
        // (WebSocketServer::broadcast() gets onto this thread via asio::post
        // before calling this).
        void enqueueSend(const std::string& text)
        {
            bool writeInProgress = !m_writeQueue.empty();
            m_writeQueue.push_back(text);
            if (!writeInProgress)
            {
                doWrite();
            }
        }

        void setOnDisconnected(std::function<void()> callback)
        {
            m_onDisconnected = std::move(callback);
        }

    private:
        void readLoop()
        {
            m_ws.async_read(
                m_buffer,
                [self = shared_from_this()](beast::error_code ec, std::size_t bytesRead)
                {
                    if (ec)
                    {
                        std::cout << "[WebSocketServer] client disconnected\n";
                        if (self->m_onDisconnected)
                            self->m_onDisconnected();
                        return;
                    }

                    std::string text = beast::buffers_to_string(self->m_buffer.data());
                    self->m_buffer.consume(bytesRead);

                    std::cout << "[WebSocketServer] received: " << text << "\n";

                    if (self->m_messageHandler)
                        self->m_messageHandler(text);

                    self->readLoop();
                });
        }

        void doWrite()
        {
            m_ws.text(true);
            m_ws.async_write(
                asio::buffer(m_writeQueue.front()),
                [self = shared_from_this()](beast::error_code ec, std::size_t)
                {
                    if (ec)
                    {
                        std::cerr << "[WebSocketServer] write failed: " << ec.message() << "\n";
                        return;
                    }
                    self->m_writeQueue.pop_front();
                    if (!self->m_writeQueue.empty())
                        self->doWrite();
                });
        }

        websocket::stream<tcp::socket> m_ws;
        beast::flat_buffer m_buffer;
        WebSocketServer::MessageHandler m_messageHandler;
        std::function<void()> m_onDisconnected;
        std::deque<std::string> m_writeQueue;
        LastSnapshotStore& m_lastSnapshotStore;
    };

    // Tracks currently-connected sessions so broadcast() has something to
    // send to. Guarded by a mutex because broadcast() is called from the
    // game thread while accept/disconnect run on the I/O thread.
    class SessionRegistry
    {
    public:
        void add(const std::shared_ptr<Session>& session)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_sessions.push_back(session);
        }

        void remove(const std::shared_ptr<Session>& session)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_sessions.erase(
                std::remove_if(m_sessions.begin(), m_sessions.end(),
                    [&](const std::weak_ptr<Session>& weak) { return weak.lock() == session; }),
                m_sessions.end());
        }

        std::vector<std::shared_ptr<Session>> snapshot()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::vector<std::shared_ptr<Session>> alive;
            for (const auto& weak : m_sessions)
            {
                if (auto locked = weak.lock())
                    alive.push_back(locked);
            }
            return alive;
        }

    private:
        std::mutex m_mutex;
        std::vector<std::weak_ptr<Session>> m_sessions;
    };


    // Accepts incoming TCP connections and hands each one off as a Session.
    class Listener : public std::enable_shared_from_this<Listener>
    {
    public:
        Listener(asio::io_context& ioContext, const tcp::endpoint& endpoint,
                 WebSocketServer::MessageHandler messageHandler, SessionRegistry& registry,
                 LastSnapshotStore& lastSnapshotStore)
            : m_acceptor(ioContext)
            , m_messageHandler(std::move(messageHandler))
            , m_registry(registry)
            , m_lastSnapshotStore(lastSnapshotStore)
        {
            beast::error_code ec;

            m_acceptor.open(endpoint.protocol(), ec);
            if (ec) { std::cerr << "[WebSocketServer] open failed: " << ec.message() << "\n"; return; }

            m_acceptor.set_option(asio::socket_base::reuse_address(true), ec);

            m_acceptor.bind(endpoint, ec);
            if (ec) { std::cerr << "[WebSocketServer] bind failed: " << ec.message() << "\n"; return; }

            m_acceptor.listen(asio::socket_base::max_listen_connections, ec);
            if (ec) { std::cerr << "[WebSocketServer] listen failed: " << ec.message() << "\n"; return; }
        }

        void run()
        {
            accept();
        }

    private:
        void accept()
        {
            m_acceptor.async_accept(
                [self = shared_from_this()](beast::error_code ec, tcp::socket socket)
                {
                    if (!ec)
                    {
                        auto session = std::make_shared<Session>(std::move(socket), self->m_messageHandler, self->m_lastSnapshotStore);
                        self->m_registry.add(session);
                        session->setOnDisconnected([self, session]
                        {
                            self->m_registry.remove(session);
                        });
                        session->run();
                    }
                    // Keep accepting further connections regardless of this one's outcome.
                    self->accept();
                });
        }

        tcp::acceptor m_acceptor;
        WebSocketServer::MessageHandler m_messageHandler;
        SessionRegistry& m_registry;
        LastSnapshotStore& m_lastSnapshotStore;
    };
}

struct WebSocketServer::Impl
{
    asio::io_context ioContext{ 1 };
    SessionRegistry registry;
    LastSnapshotStore lastSnapshot;
    MessageHandler messageHandler;
};

WebSocketServer::WebSocketServer(unsigned short port)
    : m_port(port)
    , m_impl(std::make_unique<Impl>())
{
}

WebSocketServer::~WebSocketServer()
{
    stop();
}

void WebSocketServer::setMessageHandler(MessageHandler handler)
{
    m_impl->messageHandler = std::move(handler);
}

void WebSocketServer::start()
{
    auto endpoint = tcp::endpoint(asio::ip::make_address("0.0.0.0"), m_port);
    std::make_shared<Listener>(m_impl->ioContext, endpoint, m_impl->messageHandler, m_impl->registry, m_impl->lastSnapshot)->run();

    m_ioThread = std::thread([this]
    {
        m_impl->ioContext.run();
    });

    std::cout << "[WebSocketServer] listening on port " << m_port << "\n";
}

void WebSocketServer::stop()
{
    if (!m_impl)
        return;

    m_impl->ioContext.stop();
    if (m_ioThread.joinable())
        m_ioThread.join();
}

void WebSocketServer::broadcast(const std::string& text)
{
    // Remember it regardless of whether anyone is connected right now, so
    // the next client to connect can be caught up immediately (see Session
    // ::run()'s handshake callback).
    m_impl->lastSnapshot.set(text);

    // Called from the game thread - marshal onto the I/O thread before
    // touching any Session, since Session::enqueueSend() assumes it's only
    // ever called there.
    asio::post(m_impl->ioContext, [this, text]
    {
        for (const auto& session : m_impl->registry.snapshot())
        {
            session->enqueueSend(text);
        }
    });
}
