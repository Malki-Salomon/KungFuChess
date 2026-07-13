#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include <functional>

#include "Command.h"

class CommandFactory
{
public:
    /*using Creator =
        std::function<std::unique_ptr<Command>(std::istream&)>;*/

    static std::unique_ptr<Command> create(
        std::istream& in);

    /*static void registerCommand(
        const std::string& name,
        Creator creator);*/

private:
    //static std::unordered_map<std::string, Creator> creators;
};