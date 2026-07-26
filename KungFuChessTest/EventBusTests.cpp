#include "pch.h"
#include "doctest.h"

#include "Events\EventBus.h"
#include "Events\GameStateChangedEvent.h"
#include "GameSnapshot.h"

#include <stdexcept>


TEST_SUITE("EventBus Tests")
{

    TEST_CASE("Publishing event with no subscribers does not crash")
    {
        EventBus bus;

        GameSnapshot snapshot;

        CHECK_NOTHROW(
            bus.publish(
                GameStateChangedEvent(snapshot)
            )
        );
    }


    TEST_CASE("Single subscriber receives event once")
    {
        EventBus bus;

        int counter = 0;

        bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                counter++;
            });


        GameSnapshot snapshot;

        bus.publish(
            GameStateChangedEvent(snapshot)
        );


        CHECK(counter == 1);
    }


    TEST_CASE("Multiple subscribers receive the same event")
    {
        EventBus bus;

        int subscriberA = 0;
        int subscriberB = 0;


        bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                subscriberA++;
            });


        bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                subscriberB++;
            });


        GameSnapshot snapshot;

        bus.publish(
            GameStateChangedEvent(snapshot)
        );


        CHECK(subscriberA == 1);
        CHECK(subscriberB == 1);
    }


    TEST_CASE("Subscriber receives only matching event type")
    {
        EventBus bus;

        bool called = false;


        class DummyEvent : public Event
        {
        };


        bus.subscribe<DummyEvent>(
            [&](const DummyEvent&)
            {
                called = true;
            });


        GameSnapshot snapshot;

        bus.publish(
            GameStateChangedEvent(snapshot)
        );


        CHECK_FALSE(called);
    }


    TEST_CASE("Same event can be published multiple times")
    {
        EventBus bus;

        int counter = 0;


        bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                counter++;
            });


        GameSnapshot snapshot;


        bus.publish(
            GameStateChangedEvent(snapshot)
        );

        bus.publish(
            GameStateChangedEvent(snapshot)
        );

        bus.publish(
            GameStateChangedEvent(snapshot)
        );


        CHECK(counter == 3);
    }


    TEST_CASE("Subscriber receives correct event payload")
    {
        EventBus bus;

        bool received = false;


        GameSnapshot expectedSnapshot;


        bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent& event)
            {
                received =
                    &event.getSnapshot()
                    != nullptr;
            });


        bus.publish(
            GameStateChangedEvent(expectedSnapshot)
        );


        CHECK(received);
    }


    TEST_CASE("Subscribers are called in registration order")
    {
        EventBus bus;

        std::vector<int> order;


        bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                order.push_back(1);
            });


        bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                order.push_back(2);
            });


        GameSnapshot snapshot;


        bus.publish(
            GameStateChangedEvent(snapshot)
        );


        CHECK(order.size() == 2);
        CHECK(order[0] == 1);
        CHECK(order[1] == 2);
    }


    TEST_CASE("EventBus supports many subscribers")
    {
        EventBus bus;

        int counter = 0;


        for (int i = 0; i < 1000; i++)
        {
            bus.subscribe<GameStateChangedEvent>(
                [&](const GameStateChangedEvent&)
                {
                    counter++;
                });
        }


        GameSnapshot snapshot;


        bus.publish(
            GameStateChangedEvent(snapshot)
        );


        CHECK(counter == 1000);
    }


    TEST_CASE("Object subscriber can receive event")
    {
        EventBus bus;


        class FakeSubscriber
        {
        public:

            bool received = false;

            void onEvent(
                const GameStateChangedEvent&)
            {
                received = true;
            }
        };


        FakeSubscriber subscriber;


        bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent& event)
            {
                subscriber.onEvent(event);
            });


        GameSnapshot snapshot;


        bus.publish(
            GameStateChangedEvent(snapshot)
        );


        CHECK(subscriber.received);
    }


    TEST_CASE("A throwing subscriber does not stop other subscribers from receiving the event")
    {
        EventBus bus;

        bool received = false;


        bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                throw std::runtime_error("subscriber A failed");
            });


        bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                received = true;
            });


        GameSnapshot snapshot;


        CHECK_NOTHROW(
            bus.publish(
                GameStateChangedEvent(snapshot)
            )
        );


        CHECK(received == true);
    }

    TEST_CASE("Subscriber receives event before unsubscribe")
    {
        EventBus bus;

        bool received = false;

        auto id = bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                received = true;
            });


        GameSnapshot snapshot;

        bus.publish(
            GameStateChangedEvent(snapshot)
        );


        CHECK(received == true);
    }


    TEST_CASE("Subscriber does not receive event after unsubscribe")
    {
        EventBus bus;

        bool received = false;


        auto id = bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                received = true;
            });


        bus.unsubscribe<GameStateChangedEvent>(id);


        GameSnapshot snapshot;

        bus.publish(
            GameStateChangedEvent(snapshot)
        );


        CHECK(received == false);
    }


    TEST_CASE("Removing one subscriber does not affect another subscriber")
    {
        EventBus bus;

        bool subscriberAReceived = false;
        bool subscriberBReceived = false;


        auto idA = bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                subscriberAReceived = true;
            });


        auto idB = bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                subscriberBReceived = true;
            });


        bus.unsubscribe<GameStateChangedEvent>(idA);


        GameSnapshot snapshot;

        bus.publish(
            GameStateChangedEvent(snapshot)
        );


        CHECK(subscriberAReceived == false);
        CHECK(subscriberBReceived == true);
    }


    TEST_CASE("Unsubscribe with non existing id does not throw")
    {
        EventBus bus;


        CHECK_NOTHROW(
            bus.unsubscribe<GameStateChangedEvent>(999999)
        );
    }


    TEST_CASE("Double unsubscribe is safe")
    {
        EventBus bus;

        bool received = false;


        auto id = bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                received = true;
            });


        bus.unsubscribe<GameStateChangedEvent>(id);

        CHECK_NOTHROW(
            bus.unsubscribe<GameStateChangedEvent>(id)
        );


        GameSnapshot snapshot;

        bus.publish(
            GameStateChangedEvent(snapshot)
        );


        CHECK(received == false);
    }


    TEST_CASE("Multiple subscriptions with same callback have different ids")
    {
        EventBus bus;

        int receivedCount = 0;


        auto callback =
            [&](const GameStateChangedEvent&)
            {
                receivedCount++;
            };


        auto id1 =
            bus.subscribe<GameStateChangedEvent>(callback);


        auto id2 =
            bus.subscribe<GameStateChangedEvent>(callback);


        CHECK(id1 != id2);


        GameSnapshot snapshot;


        bus.publish(
            GameStateChangedEvent(snapshot)
        );


        CHECK(receivedCount == 2);



        bus.unsubscribe<GameStateChangedEvent>(id1);


        receivedCount = 0;


        bus.publish(
            GameStateChangedEvent(snapshot)
        );


        CHECK(receivedCount == 1);
    }


    TEST_CASE("Publish only reaches active subscribers after removals")
    {
        EventBus bus;

        bool subscriberAReceived = false;
        bool subscriberBReceived = false;
        bool subscriberCReceived = false;


        auto idA = bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                subscriberAReceived = true;
            });


        auto idB = bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                subscriberBReceived = true;
            });


        auto idC = bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                subscriberCReceived = true;
            });



        bus.unsubscribe<GameStateChangedEvent>(idA);
        bus.unsubscribe<GameStateChangedEvent>(idC);



        GameSnapshot snapshot;


        bus.publish(
            GameStateChangedEvent(snapshot)
        );



        CHECK(subscriberAReceived == false);
        CHECK(subscriberBReceived == true);
        CHECK(subscriberCReceived == false);
    }

    TEST_CASE("A subscriber can unsubscribe itself while handling the event, without crashing")
    {
        EventBus bus;

        int counterA = 0;
        int counterB = 0;
        SubscriptionId idA = 0;

        idA = bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                counterA++;
                bus.unsubscribe<GameStateChangedEvent>(idA);
            });

        bus.subscribe<GameStateChangedEvent>(
            [&](const GameStateChangedEvent&)
            {
                counterB++;
            });

        GameSnapshot snapshot;

        CHECK_NOTHROW(
            bus.publish(GameStateChangedEvent(snapshot))
        );

        CHECK_NOTHROW(
            bus.publish(GameStateChangedEvent(snapshot))
        );

        CHECK(counterA == 1);
        CHECK(counterB == 2);
    }

}