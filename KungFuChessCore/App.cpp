#include "pch.h"
#include "App.h"
#include "TextBoardConvert.h"
#include "StringCommandConvert.h"
#include "Command.h"
#include <iostream>

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

    StringCommandConvert cmdConverter(commandLines);
    std::vector<Command> commands = cmdConverter.Convert();

    game.run(commands);
}

Game& App::getGame() {
    return game;
}