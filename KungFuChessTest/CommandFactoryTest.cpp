//#include "doctest.h"
//
//#include "CommandFactory.h"
//#include "Command.h"
//
//#include <sstream>
//#include <memory>
//#include <PrintBoardCommand.h>
//#include <WaitCommand.h>
//#include <ClickCommand.h>
//
//using namespace std;
//
//TEST_SUITE("CommandFactory")
//{
//    
//
//    TEST_CASE("Create click command")
//    {
//
//        stringstream ss("click 100 200");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd != nullptr);
//    }
//
//    TEST_CASE("Create wait command")
//    {
//        stringstream ss("wait 1500");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd != nullptr);
//    }
//
//    TEST_CASE("Create print board command")
//    {
//        stringstream ss("print board");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd != nullptr);
//    }
//
//    TEST_CASE("Unknown command")
//    {
//        stringstream ss("hello");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd == nullptr);
//    }
//
//    TEST_CASE("Empty stream")
//    {
//        stringstream ss("");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd == nullptr);
//    }
//
//    TEST_CASE("Command is case sensitive")
//    {
//        stringstream ss("Click 10 20");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd == nullptr);
//    }
//
//    TEST_CASE("Uppercase command")
//    {
//        stringstream ss("CLICK 10 20");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd == nullptr);
//    }
//
//    TEST_CASE("Click without parameters")
//    {
//        stringstream ss("click");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK_FALSE(cmd != nullptr);
//    }
//
//    TEST_CASE("Click with one parameter")
//    {
//        stringstream ss("click 100");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK_FALSE(cmd != nullptr);
//    }
//
//    TEST_CASE("Wait without parameter")
//    {
//        stringstream ss("wait");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK_FALSE(cmd != nullptr);
//    }
//
//    TEST_CASE("Print without board")
//    {
//        stringstream ss("print");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd == nullptr);
//    }
//
//    TEST_CASE("Print wrong target")
//    {
//        stringstream ss("print game");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd == nullptr);
//    }
//
//    TEST_CASE("Leading spaces")
//    {
//        stringstream ss("     click 10 20");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd != nullptr);
//    }
//
//    TEST_CASE("Trailing spaces")
//    {
//        stringstream ss("click 10 20      ");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd != nullptr);
//    }
//
//    TEST_CASE("Multiple spaces")
//    {
//        stringstream ss("click      10      20");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd != nullptr);
//    }
//
//    TEST_CASE("Negative coordinates")
//    {
//        stringstream ss("click -50 -100");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd != nullptr);
//    }
//
//    TEST_CASE("Zero wait")
//    {
//        stringstream ss("wait 0");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd != nullptr);
//    }
//
//    TEST_CASE("Negative wait")
//    {
//        stringstream ss("wait -100");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd != nullptr);
//    }
//
//    TEST_CASE("Large wait")
//    {
//        stringstream ss("wait 999999");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd != nullptr);
//    }
//
//    TEST_CASE("Garbage command")
//    {
//        stringstream ss("@@@");
//
//        auto cmd = CommandFactory::create(ss);
//
//        CHECK(cmd == nullptr);
//    }
//
//}