#pragma once

#include <memory>

class IGameController;

// Responsible for creating fully set-up game controllers.
// GameSession/SessionManager should not know how a game is built
// (starting position, loaded save, replay, etc.) - that knowledge lives here.
namespace GameFactory
{
    std::unique_ptr<IGameController> createStandardGame();
}
