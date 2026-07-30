#include "doctest.h"
#include "JumpMessage.h"
#include "MoveMessage.h"

TEST_SUITE("JumpMessage")
{
    TEST_CASE("build/parse round-trip")
    {
        std::string raw = JumpMessage::build("e2");

        std::string square;
        CHECK(JumpMessage::parse(raw, square));
        CHECK(square == "e2");
    }

    TEST_CASE("round-trips every square on the board")
    {
        for (char file = 'a'; file <= 'h'; ++file)
        {
            for (char rank = '1'; rank <= '8'; ++rank)
            {
                std::string expected = std::string(1, file) + std::string(1, rank);

                std::string square;
                CHECK(JumpMessage::parse(JumpMessage::build(expected), square));
                CHECK(square == expected);
            }
        }
    }

    TEST_CASE("parse rejects a message of the wrong type")
    {
        std::string square;
        CHECK_FALSE(JumpMessage::parse("{\"type\":\"board\",\"cells\":[]}", square));
    }

    // A jump and a move must never be readable as each other - that
    // mutual exclusivity is what lets the server check one before the
    // other without having to arbitrate between them.
    TEST_CASE("a move message is not a jump, and vice versa")
    {
        std::string square;
        CHECK_FALSE(JumpMessage::parse(MoveMessage::build("e2", "e4"), square));

        std::string from, to;
        CHECK_FALSE(MoveMessage::parse(JumpMessage::build("e2"), from, to));
    }

    TEST_CASE("parse rejects a jump message with no square field")
    {
        std::string square;
        CHECK_FALSE(JumpMessage::parse("{\"type\":\"jump\"}", square));
    }
}
