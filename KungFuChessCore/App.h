#pragma once
#include <string>
#include "Command.h"

#include <vector>
#include "Game.h" // ה-App מכיל מופע של ה-Game

class App {
public:

    void parseLoad(const std::vector<std::string>& inputLines);
    Game& getGame(); // פונקציה שמחזירה הפניה למופע ה-Game

private:
    Game game;
};

