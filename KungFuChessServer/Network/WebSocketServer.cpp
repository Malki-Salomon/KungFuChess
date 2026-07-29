#include "WebSocketServer.h"

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <algorithm>
#include <atomic>
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
    // Remembers the most recent broadcast text so a connection that logs
    // in *after* a broadcast already happened (the common case - the
    // initial board is broadcast once, immediately, likely before anyone
    // is even connected yet) can still be caught up right at login,
    // instead of only seeing whatever changes happen after. Read back via
    // WebSocketServer::getLastSnapshot() - see AuthWorker, the one caller.
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
    // messages (handed to WebSocketServer's MessageHandler, tagged with
    // this session's id) and can be told to send text out (queued so only
    // one async_write is ever in flight, as Beast requires).
    //
    // The whole io_context this runs on has concurrency hint 1 and only
    // ever one thread calls run() on it (see WebSocketServer::start()), so
    // every handler below - reads, writes, and posted broadcasts/sendTo
    // alike - executes serially on that single thread. No additional
    // locking is needed inside Session itself for that reason.
    //
    // Deliberately does NOT catch a freshly-connected client up on the
    // last broadcast anymore - a connection gets nothing at all until it
    // logs in (see WebSocketServer::setAuthenticationCheck()); the catch-up
    // now happens explicitly, from AuthWorker, at the moment login
    // succeeds (via WebSocketServer::getLastSnapshot() + sendTo()).
    class Session : public std::enable_shared_from_this<Session>
    {
    public:
        Session(tcp::socket socket, SessionId id, WebSocketServer::MessageHandler messageHandler)
            : m_ws(std::move(socket))
            , m_id(id)
            , m_messageHandler(std::move(messageHandler))
        {
        }

        SessionId id() const { return m_id; }

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
                    std::cout << "[WebSocketServer] client connected (session " << self->m_id << ")\n";

                    if (self->m_onConnected)
                        self->m_onConnected();

                    self->readLoop();
                });
        }

        // Queues text for sending; safe to call from the I/O thread only
        // (WebSocketServer::broadcast()/sendTo() get onto this thread via
        // asio::post before calling this).
        void enqueueSend(const std::string& text)
        {
            bool writeInProgress = !m_writeQueue.empty();
            m_writeQueue.push_back(text);
            if (!writeInProgress)
            {
                doWrite();
            }
        }

        void setOnConnected(std::function<void()> callback)
        {
            m_onConnected = std::move(callback);
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
                        std::cout << "[WebSocketServer] client disconnected (session " << self->m_id << ")\n";
                        if (self->m_onDisconnected)
                            self->m_onDisconnected();
                        return;
                    }

                    std::string text = beast::buffers_to_string(self->m_buffer.data());
                    self->m_buffer.consume(bytesRead);

                    std::cout << "[WebSocketServer] received: " << text << "\n";

                    if (self->m_messageHandler)
                        self->m_messageHandler(self->m_id, text);

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
        SessionId m_id;
        beast::flat_buffer m_buffer;
        WebSocketServer::MessageHandler m_messageHandler;
        std::function<void()> m_onConnected;
        std::function<void()> m_onDisconnected;
        std::deque<std::string> m_writeQueue;
    };

    // Tracks currently-connected sessions so broadcast()/sendTo() have
    // something to send to. Guarded by a mutex because broadcast()/sendTo()
    // are called from the game thread while accept/disconnect run on the
    // I/O thread.
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

        std::shared_ptr<Session> find(SessionId id)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (const auto& weak : m_sessions)
            {
                if (auto locked = weak.lock())
                {
                    if (locked->id() == id)
                        return locked;
                }
            }
            return nullptr;
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
                 WebSocketServer::ConnectHandler connectHandler,
                 WebSocketServer::MessageHandler messageHandler,
                 WebSocketServer::DisconnectHandler disconnectHandler,
                 SessionRegistry& registry)
            : m_acceptor(ioContext)
            , m_connectHandler(std::move(connectHandler))
            , m_messageHandler(std::move(messageHandler))
            , m_disconnectHandler(std::move(disconnectHandler))
            , m_registry(registry)
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
                        SessionId id = self->m_nextId.fetch_add(1);
                        auto session = std::make_shared<Session>(std::move(socket), id, self->m_messageHandler);
                        self->m_registry.add(session);

                        if (self->m_connectHandler)
                        {
                            auto handler = self->m_connectHandler;
                            session->setOnConnected([handler, id] { handler(id); });
                        }

                        auto disconnectHandler = self->m_disconnectHandler;
                        session->setOnDisconnected([self, session, id, disconnectHandler]
                        {
                            self->m_registry.remove(session);
                            if (disconnectHandler)
                                disconnectHandler(id);
                        });

                        session->run();
                    }
                    // Keep accepting further connections regardless of this one's outcome.
                    self->accept();
                });
        }

        tcp::acceptor m_acceptor;
        WebSocketServer::ConnectHandler m_connectHandler;
        WebSocketServer::MessageHandler m_messageHandler;
        WebSocketServer::DisconnectHandler m_disconnectHandler;
        SessionRegistry& m_registry;
        std::atomic<SessionId> m_nextId{ 1 };
    };
}

struct WebSocketServer::Impl
{
    asio::io_context ioContext{ 1 };
    SessionRegistry registry;
    LastSnapshotStore lastSnapshot;
    ConnectHandler connectHandler;
    MessageHandler messageHandler;
    DisconnectHandler disconnectHandler;
    AuthenticationCheck authenticationCheck;
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

void WebSocketServer::setConnectHandler(ConnectHandler handler)
{
    m_impl->connectHandler = std::move(handler);
}

void WebSocketServer::setMessageHandler(MessageHandler handler)
{
    m_impl->messageHandler = std::move(handler);
}

void WebSocketServer::setDisconnectHandler(DisconnectHandler handler)
{
    m_impl->disconnectHandler = std::move(handler);
}

void WebSocketServer::setAuthenticationCheck(AuthenticationCheck check)
{
    m_impl->authenticationCheck = std::move(check);
}

void WebSocketServer::start()
{
    auto endpoint = tcp::endpoint(asio::ip::make_address("0.0.0.0"), m_port);
    std::make_shared<Listener>(m_impl->ioContext, endpoint,
        m_impl->connectHandler, m_impl->messageHandler, m_impl->disconnectHandler,
        m_impl->registry)->run();

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
    // a connection that logs in later can be caught up immediately (see
    // AuthWorker, which reads it back via getLastSnapshot()).
    m_impl->lastSnapshot.set(text);

    // Called from the game thread - marshal onto the I/O thread before
    // touching any Session, since Session::enqueueSend() assumes it's only
    // ever called there.
    asio::post(m_impl->ioContext, [this, text]
    {
        for (const auto& session : m_impl->registry.snapshot())
        {
            if (m_impl->authenticationCheck && !m_impl->authenticationCheck(session->id()))
                continue; // not logged in - as far as broadcasts go, this connection doesn't exist yet

            session->enqueueSend(text);
        }
    });
}

void WebSocketServer::sendTo(SessionId id, const std::string& text)
{
    asio::post(m_impl->ioContext, [this, id, text]
    {
        if (auto session = m_impl->registry.find(id))
        {
            session->enqueueSend(text);
        }
        // If not found, the session already disconnected - nothing to do.
    });
}

std::string WebSocketServer::getLastSnapshot() const
{
    return m_impl->lastSnapshot.get();
}
