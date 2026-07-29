#include "doctest.h"
#include "PendingDisconnectTracker.h"

TEST_SUITE("PendingDisconnectTracker")
{
    TEST_CASE("hasPending is false with nothing tracked")
    {
        PendingDisconnectTracker tracker;

        CHECK_FALSE(tracker.hasPending());
    }

    TEST_CASE("begin() makes hasPending true and pending() reflects it")
    {
        PendingDisconnectTracker tracker;
        auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(20);

        tracker.begin(Seat::Second, "alice", deadline);

        CHECK(tracker.hasPending());
        PendingDisconnect p = tracker.pending();
        CHECK(p.seat == Seat::Second);
        CHECK(p.username == "alice");
        CHECK(p.deadline == deadline);
    }

    TEST_CASE("clear() removes the pending state")
    {
        PendingDisconnectTracker tracker;
        tracker.begin(Seat::First, "bob", std::chrono::steady_clock::now());

        tracker.clear();

        CHECK_FALSE(tracker.hasPending());
    }

    TEST_CASE("tryReclaim with no pending disconnect returns nullopt and changes nothing")
    {
        PendingDisconnectTracker tracker;

        auto result = tracker.tryReclaim("anyone");

        CHECK_FALSE(result.has_value());
        CHECK_FALSE(tracker.hasPending());
    }

    TEST_CASE("tryReclaim with a non-matching username returns nullopt and leaves the pending state intact")
    {
        PendingDisconnectTracker tracker;
        tracker.begin(Seat::First, "alice", std::chrono::steady_clock::now() + std::chrono::seconds(20));

        auto result = tracker.tryReclaim("mallory");

        CHECK_FALSE(result.has_value());
        CHECK(tracker.hasPending()); // unchanged - still waiting for the right username
    }

    TEST_CASE("tryReclaim with a matching username returns the seat and clears the pending state")
    {
        PendingDisconnectTracker tracker;
        tracker.begin(Seat::Second, "alice", std::chrono::steady_clock::now() + std::chrono::seconds(20));

        auto result = tracker.tryReclaim("alice");

        CHECK(result.has_value());
        CHECK(*result == Seat::Second);
        CHECK_FALSE(tracker.hasPending());
    }
}
