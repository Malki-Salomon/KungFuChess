#pragma once

enum class CommandType { Click, Wait, Print, Jump };

struct Command {
    CommandType type;
    int x = 0, y = 0;      
    long long ms = 0;   

    static Command CreateClick(int x, int y) { return { CommandType::Click, x, y, 0 }; }
    static Command CreateJump(int x, int y) { return { CommandType::Jump, x, y, 0 }; }
    static Command CreateWait(long long ms) { return { CommandType::Wait, 0, 0, ms }; }
    static Command CreatePrint() { return { CommandType::Print, 0, 0, 0 }; }
};

//#pragma once
//#include <iostream>
//#include <memory>
//
//class Game;
//
//class Command
//{
//public:
//    virtual ~Command() = default;
//
//    virtual void execute(Game& game) = 0;
//};