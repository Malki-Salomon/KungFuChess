#include "pch.h"
#include "StringCommandConvert.h"

#include <sstream>
#include "IBoardPrinter.h"

std::vector<Command> StringCommandConvert::Convert() {
    std::vector<Command> result;
    for (const auto& line : input) {
        std::stringstream ss(line);
        std::string name;
        ss >> name;

        if (name == "click") {
            int x, y; ss >> x >> y;
            result.push_back(Command::CreateClick(x, y));
        }
        else if(name == "jump") {
            int x, y; ss >> x >> y;
            result.push_back(Command::CreateJump(x, y));
		}
        else if (name == "wait") {
            long long ms; ss >> ms;
            result.push_back(Command::CreateWait(ms));
        }
        else if (name == "print") {
            result.push_back(Command::CreatePrint());
        }
    }
    return result;
}
//using namespace std;
//
//StringCommandConvert::StringCommandConvert(
//    const vector<string>& input)
//    : input(input)
//{
//}
//
//vector<unique_ptr<Command>>
//StringCommandConvert::Convert()
//{
//    vector<unique_ptr<Command>> result;
//
//    for (const auto& line : input)
//    {
//        stringstream ss(line);
//
//        auto cmd = CommandFactory::create(ss);
//
//        if (cmd)
//        {
//            result.push_back(move(cmd));
//        }
//    }
//
//    return result;
//}