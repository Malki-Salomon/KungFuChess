#include "doctest.h"
#include "OpponentDisconnectedMessage.h"
#include "OpponentReconnectedMessage.h"

TEST_SUITE("Disconnect/reconnect protocol messages")
{
    TEST_CASE("OpponentDisconnectedMessage build/parse round-trip")
    {
        std::string raw = OpponentDisconnectedMessage::build(20);

        int seconds = 0;
        CHECK(OpponentDisconnectedMessage::parse(raw, seconds));
        CHECK(seconds == 20);
    }

    TEST_CASE("OpponentDisconnectedMessage::parse rejects a message of the wrong type")
    {
        int seconds = 0;
        CHECK_FALSE(OpponentDisconnectedMessage::parse("{\"type\":\"opponentReconnected\"}", seconds));
    }

    TEST_CASE("OpponentReconnectedMessage build/parse round-trip")
    {
        std::string raw = OpponentReconnectedMessage::build();

        CHECK(OpponentReconnectedMessage::parse(raw));
    }

    TEST_CASE("OpponentReconnectedMessage::parse rejects a message of the wrong type")
    {
        CHECK_FALSE(OpponentReconnectedMessage::parse("{\"type\":\"opponentDisconnected\",\"secondsRemaining\":20}"));
    }
}
