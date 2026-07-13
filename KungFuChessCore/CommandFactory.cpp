#include "pch.h"
#include "CommandFactory.h"
#include "ClickCommand.h"
#include "WaitCommand.h"
#include "PrintBoardCommand.h"

using namespace std;

unique_ptr<Command> CommandFactory::create(istream& in)
{
    string name;

    if (!(in >> name))
    {
        return nullptr;
    }

    if (name == "click")
    {
        int x, y;

        if (!(in >> x >> y))
            return nullptr;

        return make_unique<ClickCommand>(x, y);
    }

    if (name == "wait")
    {
        long long ms;

        if (!(in >> ms))
            return nullptr;

        return make_unique<WaitCommand>(ms);
    }

    if (name == "print")
    {
        string what;

        if (!(in >> what) || what != "board")
            return nullptr;

        return make_unique<PrintBoardCommand>();
    }

    return nullptr;
}

//unordered_map<string, CommandFactory::Creator>
//CommandFactory::creators;

//void CommandFactory::registerCommand(
//    const string& name,
//    Creator creator)
//{
//    creators[name] = creator;
//}

//unique_ptr<Command> CommandFactory::create(
//    istream& in)
//{
//    string name;
//    in >> name;
//
//    auto it = creators.find(name);
//
//    if (it == creators.end())
//        return nullptr;
//
//    return it->second(in);
//}