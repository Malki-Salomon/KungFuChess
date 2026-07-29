#pragma once

#include "GameStatus.h"
#include "../Network/SessionId.h"

#include <vector>

class GameSession;
class PlayerAssignment;
class PlayerDirectory;
class MatchResultService;
class WebSocketServer;

// The one place that closes the loop once Core reports a finished game:
// detects the Playing -> finished transition (GameSession::getStatus()),
// works out who won, updates ratings, and sends a gameOver message to the
// room's members. Deliberately its own class, called as a single line
// from Server::run(), rather than inlined into that loop's body - see the
// comment on Server::run() about not growing it further.
class GameEndCoordinator
{
public:
    // roomMembers is a reference to the owning Room's own member list -
    // gameOver reaches that room's players/spectators only, via
    // WebSocketServer::sendToMany(), never every connection server-wide.
    GameEndCoordinator(GameSession& gameSession,
                        PlayerAssignment& playerAssignment,
                        PlayerDirectory& playerDirectory,
                        MatchResultService& matchResultService,
                        WebSocketServer& webSocketServer,
                        const std::vector<SessionId>& roomMembers);

    // Called once per server tick. A no-op unless the game has just
    // transitioned away from GameStatus::Playing for the first time -
    // there's only one game per Room, so the internal "already handled"
    // flag never needs to reset.
    void checkAndHandle();

private:
    GameSession& m_gameSession;
    PlayerAssignment& m_playerAssignment;
    PlayerDirectory& m_playerDirectory;
    MatchResultService& m_matchResultService;
    WebSocketServer& m_webSocketServer;
    const std::vector<SessionId>& m_roomMembers;

    bool m_alreadyHandled = false;
};
