#include "Command.h"
#include <vector>
#include <string>

class StringCommandConvert {
    std::vector<std::string> input;
public:
    StringCommandConvert(const std::vector<std::string>& input) : input(input) {}
    std::vector<Command> Convert(); 
};

//#pragma once
//
//#include "ICommandConvert.h"
//#include <string>
//#include <vector>
//
//class StringCommandConvert : public ICommandConvert
//{
//private:
//    std::vector<std::string> input;
//
//public:
//    StringCommandConvert(
//        const std::vector<std::string>& input);
//
//    std::vector<std::unique_ptr<Command>>
//        Convert() override;
//};