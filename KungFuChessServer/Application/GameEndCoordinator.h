#pragma once

#include "GameStatus.h"
#include "PieceTypes.h"
#include "../Network/SessionId.h"

#include <string>
#include <vector>

class GameSession;
class PlayerAssignment;
class PlayerDirectory;
class MatchResultService;
class WebSocketServer;
enum class MatchOutcome;

// The one place that closes the loop once a game ends, however it ends:
// detects a Core-detected Playing -> finished transition
// (GameSession::getStatus()) via checkAndHandle(), or a forced
// resignation (e.g. the opponent didn't reconnect in time) via
// forceResign() - either way, works out who won, updates ratings, and
// sends a gameOver message to the room's members. Deliberately its own
// class, called as a single line from Server::run(), rather than inlined
// into that loop's body - see the comment on Server::run() about not
// growing it further.
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

    // Ends the game right now with the given color as winner
    // (resignation - e.g. the opponent didn't reconnect within the
    // reconnect grace period), running the exact same ratings-update +
    // gameOver-broadcast flow checkAndHandle() uses for a real
    // checkmate. Safe to call at most meaningfully once - reuses the
    // same "already handled" guard so a real Core-detected result
    // afterward (unlikely, but possible) doesn't double-fire.
    void forceResign(PieceColor winner);

private:
    void finishGame(const std::string& resultName, MatchOutcome outcome);

    GameSession& m_gameSession;
    PlayerAssignment& m_playerAssignment;
    PlayerDirectory& m_playerDirectory;
    MatchResultService& m_matchResultService;
    WebSocketServer& m_webSocketServer;
    const std::vector<SessionId>& m_roomMembers;

    bool m_alreadyHandled = false;
};
