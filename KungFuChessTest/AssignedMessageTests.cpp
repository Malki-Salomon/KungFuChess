#include "doctest.h"
#include "AssignedMessage.h"

TEST_SUITE("AssignedMessage")
{
    TEST_CASE("build/parse round-trip for white")
    {
        std::string raw = AssignedMessage::build("white");

        std::string color;
        CHECK(AssignedMessage::parse(raw, color));
        CHECK(color == "white");
    }

    TEST_CASE("build/parse round-trip for black")
    {
        std::string raw = AssignedMessage::build("black");

        std::string color;
        CHECK(AssignedMessage::parse(raw, color));
        CHECK(color == "black");
    }

    TEST_CASE("build/parse round-trip for spectator")
    {
        std::string raw = AssignedMessage::build("spectator");

        std::string color;
        CHECK(AssignedMessage::parse(raw, color));
        CHECK(color == "spectator");
    }

    TEST_CASE("parse rejects a message of the wrong type")
    {
        std::string color;
        CHECK_FALSE(AssignedMessage::parse("{\"type\":\"board\",\"cells\":[]}", color));
    }
}
