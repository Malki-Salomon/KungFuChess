#include "pch.h"
#include "App.h"
#include "TextBoardConvert.h"
#include "StringCommandConvert.h"
#include "IBoardPrinter.h"
#include "Command.h"
#include <iostream>
#include <sstream>

void App::parseLoad(const std::vector<std::string>& inputLines) {
    std::vector<std::string> boardLines;
    std::vector<std::string> commandLines;
    bool parsingCommands = false;

    for (const auto& line : inputLines) {
        if (line == "Commands:") { parsingCommands = true; continue; }
        if (line == " Board:") { continue; }
        if (line.empty()) continue;

        if (!parsingCommands) boardLines.push_back(line);
        else commandLines.push_back(line);
    }

    TextBoardConvert boardConverter(boardLines);
    game.setupBoard(boardConverter);

    processNewCommands(commandLines);
}

void App::processNewCommands(const std::vector<std::string>& commandLines) {
    StringCommandConvert cmdConverter(commandLines);
    std::vector<Command> commands = cmdConverter.Convert();

    for (const auto& cmd : commands) {
        game.addCommand(cmd);
    }
}

void App::dispatchCommand(const std::string& cmd) {
    processNewCommands({ cmd });
}

void App::setOutputDevice(IBoardPrinter* printer) {
    game.setPrinter(printer);
}

void App::run() 
{
    while (game.isGameActive())
    {
		game.processNextCommand();
    }
}

Game& App::getGame() {
    return game;
}