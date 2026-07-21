#pragma once
#include <string>
#include "Command.h"
#include <vector>
#include "Game.h"
#include "IGameController.h"

class IBoardPrinter;

class App : public IGameController {
public:

	void run() override;
    void parseLoad(const std::vector<std::string>& inputLines) override;
    void processNewCommands(const std::vector<std::string>& commandLines);
    void setOutputDevice(IBoardPrinter* printer) override;
    Game& getGame();

    void dispatchCommand(const std::string& cmd) override;

private:
    Game game;
};

