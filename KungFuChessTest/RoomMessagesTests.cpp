#include "doctest.h"
#include "CreateRoomMessage.h"
#include "JoinRoomMessage.h"
#include "RoomCreatedMessage.h"
#include "RoomJoinedMessage.h"
#include "RoomJoinFailedMessage.h"

TEST_SUITE("Room protocol messages")
{
    TEST_CASE("CreateRoomMessage build/parse round-trip")
    {
        std::string raw = CreateRoomMessage::build();

        CHECK(CreateRoomMessage::parse(raw));
    }

    TEST_CASE("CreateRoomMessage::parse rejects a message of the wrong type")
    {
        CHECK_FALSE(CreateRoomMessage::parse("{\"type\":\"joinRoom\",\"code\":\"1\"}"));
    }

    TEST_CASE("JoinRoomMessage build/parse round-trip")
    {
        std::string raw = JoinRoomMessage::build("3");

        std::string code;
        CHECK(JoinRoomMessage::parse(raw, code));
        CHECK(code == "3");
    }

    TEST_CASE("RoomCreatedMessage build/parse round-trip")
    {
        std::string raw = RoomCreatedMessage::build("3");

        std::string code;
        CHECK(RoomCreatedMessage::parse(raw, code));
        CHECK(code == "3");
    }

    TEST_CASE("RoomJoinedMessage build/parse round-trip for a player")
    {
        std::string raw = RoomJoinedMessage::build("3", "player");

        std::string code, role;
        CHECK(RoomJoinedMessage::parse(raw, code, role));
        CHECK(code == "3");
        CHECK(role == "player");
    }

    TEST_CASE("RoomJoinedMessage build/parse round-trip for a spectator")
    {
        std::string raw = RoomJoinedMessage::build("3", "spectator");

        std::string code, role;
        CHECK(RoomJoinedMessage::parse(raw, code, role));
        CHECK(code == "3");
        CHECK(role == "spectator");
    }

    TEST_CASE("RoomJoinFailedMessage build/parse round-trip")
    {
        std::string raw = RoomJoinFailedMessage::build("room not found");

        std::string reason;
        CHECK(RoomJoinFailedMessage::parse(raw, reason));
        CHECK(reason == "room not found");
    }

    TEST_CASE("Each message type's parse rejects the others")
    {
        std::string roomCreated = RoomCreatedMessage::build("1");

        std::string code, role, reason;
        CHECK_FALSE(JoinRoomMessage::parse(roomCreated, code));
        CHECK_FALSE(RoomJoinedMessage::parse(roomCreated, code, role));
        CHECK_FALSE(RoomJoinFailedMessage::parse(roomCreated, reason));
        CHECK_FALSE(CreateRoomMessage::parse(roomCreated));
    }
}
