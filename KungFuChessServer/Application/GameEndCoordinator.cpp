#include "GameEndCoordinator.h"

#include "GameSession.h"
#include "PlayerAssignment.h"
#include "PlayerDirectory.h"
#include "MatchResultService.h"
#include "../Network/WebSocketServer.h"
#include "GameOverMessage.h"

namespace
{
    // The only place that decides what a finished GameStatus means in
    // MatchResultService's own vocabulary - MatchResultService itself
    // knows nothing about Core (see MatchResultService.h), same reasoning
    // as colorForSeat() in Server.cpp translating Seat into PieceColor.
    MatchOutcome outcomeForStatus(GameStatus status)
    {
        switch (status)
        {
            case GameStatus::WhiteWins: return MatchOutcome::WhiteWon;
            case GameStatus::BlackWins: return MatchOutcome::BlackWon;
            default:                    return MatchOutcome::Draw;
        }
    }

    std::string resultNameForStatus(GameStatus status)
    {
        switch (status)
        {
            case GameStatus::WhiteWins: return "white";
            case GameStatus::BlackWins: return "black";
            default:                    return "draw";
        }
    }
}

GameEndCoordinator::GameEndCoordinator(GameSession& gameSession,
                                        PlayerAssignment& playerAssignment,
                                        PlayerDirectory& playerDirectory,
                                        MatchResultService& matchResultService,
                                        WebSocketServer& webSocketServer)
    : m_gameSession(gameSession)
    , m_playerAssignment(playerAssignment)
    , m_playerDirectory(playerDirectory)
    , m_matchResultService(matchResultService)
    , m_webSocketServer(webSocketServer)
{
}

void GameEndCoordinator::checkAndHandle()
{
    if (m_alreadyHandled)
        return;

    GameStatus status = m_gameSession.getStatus();
    if (status == GameStatus::Playing)
        return;

    m_alreadyHandled = true;

    SessionId whiteSession = m_playerAssignment.sessionIdForSeat(Seat::First);
    SessionId blackSession = m_playerAssignment.sessionIdForSeat(Seat::Second);

    std::string whiteUsername = whiteSession != kInvalidSessionId ? m_playerDirectory.usernameOf(whiteSession) : std::string();
    std::string blackUsername = blackSession != kInvalidSessionId ? m_playerDirectory.usernameOf(blackSession) : std::string();

    bool haveRatings = !whiteUsername.empty() && !blackUsername.empty();
    int whiteNewRating = 0;
    int blackNewRating = 0;

    if (haveRatings)
    {
        MatchResult result = m_matchResultService.recordResult(whiteUsername, blackUsername, outcomeForStatus(status));
        whiteNewRating = result.whiteNewRating;
        blackNewRating = result.blackNewRating;
    }

    m_webSocketServer.broadcast(GameOverMessage::build(resultNameForStatus(status), haveRatings, whiteNewRating, blackNewRating));
}
