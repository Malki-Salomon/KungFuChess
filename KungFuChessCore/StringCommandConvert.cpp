#include "pch.h"
#include "StringCommandConvert.h"
#include "CommandFactory.h"

#include <sstream>

using namespace std;

StringCommandConvert::StringCommandConvert(
    const vector<string>& input)
    : input(input)
{
}

vector<unique_ptr<Command>>
StringCommandConvert::Convert()
{
    vector<unique_ptr<Command>> result;

    for (const auto& line : input)
    {
        stringstream ss(line);

        auto cmd = CommandFactory::create(ss);

        if (cmd)
        {
            result.push_back(move(cmd));
        }
    }

    return result;
}