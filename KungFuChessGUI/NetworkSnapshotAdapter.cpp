#include "NetworkSnapshotAdapter.h"

#include "Events/EventBus.h"
#include "Events/GameStateChangedEvent.h"
#include "GameSnapshot.h"
#include "PieceRegistry.h"

#include "BoardMessage.h"
#include "GameOverMessage.h"
#include "AssignedMessage.h"
#include "OpponentDisconnectedMessage.h"
#include "OpponentReconnectedMessage.h"
#include "Logger.h"

#include <iostream>

NetworkSnapshotAdapter::NetworkSnapshotAdapter(EventBus& eventBus)
    : eventBus(eventBus)
{
    registerHandlers();
}

void NetworkSnapshotAdapter::registerHandlers()
{
    // Order only decides who gets asked first; each handler's own parse()
    // rejects messages that aren't its type, so a new entry appended here
    // never disturbs the existing ones.
    handlers.push_back([this](const std::string& raw) { return handleBoard(raw); });
    handlers.push_back([this](const std::string& raw) { return handleGameOver(raw); });
    handlers.push_back([this](const std::string& raw) { return handleAssigned(raw); });
    handlers.push_back([this](const std::string& raw) { return handleOpponentDisconnected(raw); });
    handlers.push_back([this](const std::string& raw) { return handleOpponentReconnected(raw); });
}

void NetworkSnapshotAdapter::onMessage(const std::string& rawMessage)
{
    for (const auto& handler : handlers)
    {
        if (handler(rawMessage))
            return;
    }

    // Not a message type this client acts on - ignore it rather than
    // treating it as an error, so the server can add message types without
    // breaking older clients.
}

bool NetworkSnapshotAdapter::handleBoard(const std::string& rawMessage)
{
    std::vector<std::vector<std::string>> rows;
    if (!BoardMessage::parse(rawMessage, rows))
        return false;

    // Built up in full first, and only published once every cell has
    // parsed - so a single bad symbol can't leave a half-updated board on
    // screen.
    GameSnapshot snapshot;
    snapshot.cells.resize(rows.size());

    for (size_t row = 0; row < rows.size(); ++row)
    {
        snapshot.cells[row].resize(rows[row].size());

        for (size_t col = 0; col < rows[row].size(); ++col)
        {
            const std::string& symbol = rows[row][col];

            // "." is a legitimately empty square, not a parse failure -
            // PieceRegistry only knows real pieces, so empties are handled
            // here rather than being fed to tryParseSymbol.
            if (symbol == ".")
            {
                snapshot.cells[row][col] = PieceSnapshot{ PieceType::Empty, PieceColor::None };
                continue;
            }

            PieceType type;
            PieceColor color;
            if (!PieceRegistry::tryParseSymbol(symbol, type, color))
            {
                Logger::error("[NetworkSnapshotAdapter] unrecognized board symbol \"" + symbol +
                    "\" - discarding the entire snapshot");
                return true; // claimed as a board message, just an unusable one
            }

            snapshot.cells[row][col] = PieceSnapshot{ type, color };
        }
    }

    eventBus.publish(GameStateChangedEvent(snapshot));
    return true;
}

bool NetworkSnapshotAdapter::handleGameOver(const std::string& rawMessage)
{
    std::string result;
    bool hasRatings = false;
    int whiteNewRating = 0;
    int blackNewRating = 0;
    if (!GameOverMessage::parse(rawMessage, result, hasRatings, whiteNewRating, blackNewRating))
        return false;

    std::string summary = "Game over: " + result + " wins";
    if (hasRatings)
    {
        summary += " (white -> " + std::to_string(whiteNewRating) +
                   ", black -> " + std::to_string(blackNewRating) + ")";
    }

    // A console line is the minimal honest handler for now - showing this
    // inside the window itself is a future enhancement.
    std::cout << summary << "\n";
    Logger::info("[NetworkSnapshotAdapter] " + summary);
    return true;
}

bool NetworkSnapshotAdapter::handleAssigned(const std::string& rawMessage)
{
    std::string color;
    if (!AssignedMessage::parse(rawMessage, color))
        return false;

    std::cout << "You are playing as: " << color << "\n";
    Logger::info("[NetworkSnapshotAdapter] assigned seat: " + color);
    return true;
}

bool NetworkSnapshotAdapter::handleOpponentDisconnected(const std::string& rawMessage)
{
    int secondsRemaining = 0;
    if (!OpponentDisconnectedMessage::parse(rawMessage, secondsRemaining))
        return false;

    std::cout << "Opponent disconnected - " << secondsRemaining
              << "s to reconnect before resigning.\n";
    Logger::info("[NetworkSnapshotAdapter] opponent disconnected, " +
        std::to_string(secondsRemaining) + "s remaining");
    return true;
}

bool NetworkSnapshotAdapter::handleOpponentReconnected(const std::string& rawMessage)
{
    if (!OpponentReconnectedMessage::parse(rawMessage))
        return false;

    std::cout << "Opponent reconnected.\n";
    Logger::info("[NetworkSnapshotAdapter] opponent reconnected");
    return true;
}
