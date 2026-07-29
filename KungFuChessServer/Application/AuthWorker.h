#pragma once

#include "PlayerDirectory.h"
#include "AuthService.h"
#include "../Network/SessionId.h"
#include "../Network/WebSocketServer.h"

#include <condition_variable>
#include <deque>
#include <mutex>
#include <string>
#include <thread>

// What gets queued for AuthWorker to process. Resolved from the raw wire
// message on the network thread (LoginMessage::parse()/RegisterMessage::
// parse()) before handing off - AuthWorker itself has no protocol/JSON
// knowledge, same division of labor CommandInbox already uses for moves.
enum class AuthRequestType { Register, Login };

struct AuthRequest
{
    SessionId sender;
    AuthRequestType type;
    std::string username;
    std::string password;
};

// Runs register/login work (AuthService: real Argon2id hashing + real
// SQLite disk I/O, as of stages 4b/4c) on one dedicated background
// thread - off both the network I/O thread (WebSocketServer's callbacks
// run there; blocking it stalls every connected client's reads/writes,
// see Server.cpp) and the game-tick thread (which has its own unrelated
// job - ticking RealTimeArbiter - see CommandInbox). A single thread is
// enough at this project's current scale.
//
// AuthService itself is unaffected by any of this - it's just a plain
// object now called from a different thread than before; it doesn't
// know or care which one.
//
// Shutdown: the destructor stops accepting new work but lets the worker
// thread finish draining everything already queued (not just whatever's
// in flight) before joining - so no client that already sent a
// register/login is left without a reply. WebSocketServer::sendTo()
// already no-ops safely if they've since disconnected, so this is never
// unsafe, just occasionally slightly slower to shut down.
class AuthWorker
{
public:
    AuthWorker(AuthService& authService, WebSocketServer& webSocketServer, PlayerDirectory& playerDirectory);
    ~AuthWorker();

    AuthWorker(const AuthWorker&) = delete;
    AuthWorker& operator=(const AuthWorker&) = delete;

    // Thread-safe. Called from the network I/O thread.
    void push(const AuthRequest& request);

private:
    void run();

    AuthService& m_authService;
    WebSocketServer& m_webSocketServer;
    PlayerDirectory& m_playerDirectory;

    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::deque<AuthRequest> m_pending;
    bool m_stopping = false;

    // Started last, in the constructor body - by then every other member
    // above is already fully constructed, so run() (which touches all of
    // them) never races the rest of this object's own construction.
    std::thread m_thread;
};
