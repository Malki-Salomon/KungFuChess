#include "pch.h"

#include "RegisterCommands.h"

#include "CommandFactory.h"

#include "ClickCommand.h"
#include "WaitCommand.h"
#include "PrintBoardCommand.h"

//void registerAllCommands()
//{
//    static bool initialized = false;
//
//    if (initialized)
//    {
//        return;
//    }
//
//    initialized = true;
//
//    CommandFactory::registerCommand(
//        "click",
//        ClickCommand::create);
//
//    CommandFactory::registerCommand(
//        "wait",
//        WaitCommand::create);
//
//    CommandFactory::registerCommand(
//        "print",
//        PrintBoardCommand::create);
//}