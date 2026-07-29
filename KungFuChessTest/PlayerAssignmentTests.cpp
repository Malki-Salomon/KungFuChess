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

    TEST_CASE("Two independent instances never affect each other - the per-room isolation Room relies on")
    {
        PlayerAssignment roomA;
        PlayerAssignment roomB;
        SessionId sessionInA = 11;
        SessionId sessionInB = 22;

        roomA.assign(sessionInA);

        // A seat in roomA must not exist, in any form, in roomB - not as
        // that same connection's seat, and not by shifting who holds
        // roomB's First/Second seats.
        CHECK(roomB.seatOf(sessionInA) == Seat::Spectator);
        CHECK(roomB.sessionIdForSeat(Seat::First) == kInvalidSessionId);
        CHECK(roomB.sessionIdForSeat(Seat::Second) == kInvalidSessionId);

        roomB.assign(sessionInB);

        CHECK(roomA.sessionIdForSeat(Seat::First) == sessionInA);
        CHECK(roomA.sessionIdForSeat(Seat::Second) == kInvalidSessionId);
        CHECK(roomB.sessionIdForSeat(Seat::First) == sessionInB);
    }

    TEST_CASE("bothSeatsFilled is false with zero or one seat assigned, true only once both are, false again after a release")
    {
        PlayerAssignment assignment;
        SessionId first = 11;
        SessionId second = 22;

        CHECK_FALSE(assignment.bothSeatsFilled());

        assignment.assign(first);
        CHECK_FALSE(assignment.bothSeatsFilled());

        assignment.assign(second);
        CHECK(assignment.bothSeatsFilled());

        // e.g. a disconnect releasing one seat.
        assignment.release(first);
        CHECK_FALSE(assignment.bothSeatsFilled());
    }

    TEST_CASE("reoccupySeat replaces whoever currently holds that seat - for restoring a reconnecting player")
    {
        PlayerAssignment assignment;
        SessionId original = 11;
        SessionId reconnected = 33;

        assignment.assign(original);
        assignment.reoccupySeat(Seat::First, reconnected);

        CHECK(assignment.seatOf(original) == Seat::Spectator); // old id no longer holds any seat
        CHECK(assignment.sessionIdForSeat(Seat::First) == reconnected);
    }
}
