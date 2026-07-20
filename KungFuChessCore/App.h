#pragma once
#include <string>
#include "Command.h"
#include <vector>
#include "Game.h"

class IBoardPrinter;

class App {
public:

	void run();
    void parseLoad(const std::vector<std::string>& inputLines);
    void processNewCommands(const std::vector<std::string>& commandLines);
    void setOutputDevice(IBoardPrinter* printer);
    Game& getGame();

private:
    Game game;
};

