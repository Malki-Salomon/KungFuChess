#include "doctest.h"
#include "PlayerAssignment.h"

TEST_SUITE("PlayerAssignment")
{
    TEST_CASE("sessionIdForSeat returns the session currently holding that seat")
    {
        PlayerAssignment assignment;
        SessionId first = 11;
        SessionId second = 22;

        assignment.assign(first);
        assignment.assign(second);

        CHECK(assignment.sessionIdForSeat(Seat::First) == first);
        CHECK(assignment.sessionIdForSeat(Seat::Second) == second);
    }

    TEST_CASE("sessionIdForSeat returns kInvalidSessionId for a seat nobody holds")
    {
        PlayerAssignment assignment;

        CHECK(assignment.sessionIdForSeat(Seat::First) == kInvalidSessionId);
        CHECK(assignment.sessionIdForSeat(Seat::Second) == kInvalidSessionId);
    }

    TEST_CASE("sessionIdForSeat reflects a release followed by a reassignment")
    {
        PlayerAssignment assignment;
        SessionId first = 11;
        SessionId replacement = 33;

        assignment.assign(first);
        assignment.release(first);

        CHECK(assignment.sessionIdForSeat(Seat::First) == kInvalidSessionId);

        assignment.assign(replacement);

        CHECK(assignment.sessionIdForSeat(Seat::First) == replacement);
    }

    TEST_CASE("Spectator seat has no single session id, even when spectators are connected")
    {
        PlayerAssignment assignment;
        assignment.assign(1);
        assignment.assign(2);
        assignment.assign(3); // third connection -> spectator

        CHECK(assignment.sessionIdForSeat(Seat::Spectator) == kInvalidSessionId);
    }

    TEST_CASE("assign() is idempotent: calling it again for the same id returns the same seat unchanged")
    {
        PlayerAssignment assignment;
        SessionId first = 11;
        SessionId second = 22;

        Seat firstSeat = assignment.assign(first);
        assignment.assign(second);

        Seat firstSeatAgain = assignment.assign(first);

        CHECK(firstSeatAgain == firstSeat);
        CHECK(assignment.sessionIdForSeat(Seat::First) == first);
        CHECK(assignment.sessionIdForSeat(Seat::Second) == second); // untouched by the repeat assign()
    }
}
