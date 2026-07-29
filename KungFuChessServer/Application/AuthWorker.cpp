#include "AuthWorker.h"

#include "AuthResultMessage.h"

AuthWorker::AuthWorker(AuthService& authService, WebSocketServer& webSocketServer, PlayerDirectory& playerDirectory)
    : m_authService(authService)
    , m_webSocketServer(webSocketServer)
    , m_playerDirectory(playerDirectory)
{
    m_thread = std::thread(&AuthWorker::run, this);
}

AuthWorker::~AuthWorker()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stopping = true;
    }
    m_cv.notify_all();
    m_thread.join();
}

void AuthWorker::push(const AuthRequest& request)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pending.push_back(request);
    }
    m_cv.notify_one();
}

void AuthWorker::run()
{
    while (true)
    {
        AuthRequest request;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this] { return m_stopping || !m_pending.empty(); });

            // The predicate above only returns once m_stopping is true OR
            // m_pending is non-empty (or both) - so an empty queue here
            // can only mean m_stopping is true, i.e. there's genuinely
            // nothing left to drain.
            if (m_pending.empty())
                return;

            request = m_pending.front();
            m_pending.pop_front();
        }

        AuthResult result = request.type == AuthRequestType::Register
            ? m_authService.registerAccount(request.username, request.password)
            : m_authService.login(request.username, request.password);

        m_webSocketServer.sendTo(request.sender, AuthResultMessage::build(result.success, result.message, result.rating));

        if (request.type == AuthRequestType::Login && result.success)
            m_playerDirectory.set(request.sender, request.username);
    }
}
