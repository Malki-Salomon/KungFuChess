#include "doctest.h"
#include "RoomRegistry.h"

TEST_SUITE("RoomRegistry")
{
    TEST_CASE("createRoomId returns distinct ids across multiple calls")
    {
        RoomRegistry registry;

        std::string first = registry.createRoomId();
        std::string second = registry.createRoomId();
        std::string third = registry.createRoomId();

        CHECK(first != second);
        CHECK(second != third);
        CHECK(first != third);
    }

    TEST_CASE("roomIdForSession is empty before any room is set")
    {
        RoomRegistry registry;

        CHECK(registry.roomIdForSession(1) == "");
    }

    TEST_CASE("roomIdForSession reflects setSessionRoom")
    {
        RoomRegistry registry;
        std::string roomId = registry.createRoomId();

        registry.setSessionRoom(1, roomId);

        CHECK(registry.roomIdForSession(1) == roomId);
    }

    TEST_CASE("roomIdForSession is empty again after clearSessionRoom")
    {
        RoomRegistry registry;
        std::string roomId = registry.createRoomId();

        registry.setSessionRoom(1, roomId);
        registry.clearSessionRoom(1);

        CHECK(registry.roomIdForSession(1) == "");
    }

    TEST_CASE("Different sessions can belong to different rooms independently")
    {
        RoomRegistry registry;
        std::string roomA = registry.createRoomId();
        std::string roomB = registry.createRoomId();

        registry.setSessionRoom(1, roomA);
        registry.setSessionRoom(2, roomB);

        CHECK(registry.roomIdForSession(1) == roomA);
        CHECK(registry.roomIdForSession(2) == roomB);
    }
}
