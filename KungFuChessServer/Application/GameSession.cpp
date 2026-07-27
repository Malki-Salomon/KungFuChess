#include "GameSession.h"

#include "IGameController.h"

GameSession::GameSession(std::unique_ptr<IGameController> controller)
    : m_controller(std::move(controller))
{
}

GameSession::~GameSession() = default;

void GameSession::tick()
{
    // IGameController::run() advances the game by exactly one step
    // (processes the next pending command if the game is active).
    // It is not a blocking loop, so it is safe to call once per server tick.
    m_controller->run();
}

void GameSession::dispatchCommand(const std::string& cmd)
{
    m_controller->dispatchCommand(cmd);
}

void GameSession::attachPrinter(IBoardPrinter* printer)
{
    m_controller->setOutputDevice(printer);
}

GameSnapshot GameSession::getSnapshot() const
{
    return m_controller->getSnapshot();
}
