//#include "pch.h"
//
//#include "doctest.h"
//
//#include "../KungFuChessGUI/PrinterAdapter.h"
//
//#include "Events/EventBus.h"
//#include "Events/GameStateChangedEvent.h"
//#include "GameSnapshot.h"
//
//#include <stdexcept>
//
//
//TEST_SUITE("PrinterAdapter Tests")
//{
//
//
//    TEST_CASE("Adapter publishes event when Convert is called")
//    {
//        EventBus bus;
//
//        bool received = false;
//
//        bus.subscribe<GameStateChangedEvent>(
//            [&](const GameStateChangedEvent&)
//            {
//                received = true;
//            });
//
//
//        PrinterAdapter adapter(bus);
//
//        GameSnapshot snapshot;
//
//        adapter.Convert(snapshot);
//
//
//        CHECK(received);
//    }
//
//
//
//    TEST_CASE("Adapter works when there are no subscribers")
//    {
//        EventBus bus;
//
//        PrinterAdapter adapter(bus);
//
//        GameSnapshot snapshot;
//
//
//        CHECK_NOTHROW(
//            adapter.Convert(snapshot)
//        );
//    }
//
//
//
//    TEST_CASE("Adapter handles empty snapshot")
//    {
//        EventBus bus;
//
//        PrinterAdapter adapter(bus);
//
//
//        GameSnapshot snapshot;
//
//
//        CHECK_NOTHROW(
//            adapter.Convert(snapshot)
//        );
//    }
//
//
//
//    TEST_CASE("Adapter notifies multiple subscribers")
//    {
//        EventBus bus;
//
//
//        int subscriber1 = 0;
//        int subscriber2 = 0;
//
//
//        bus.subscribe<GameStateChangedEvent>(
//            [&](const GameStateChangedEvent&)
//            {
//                subscriber1++;
//            });
//
//
//        bus.subscribe<GameStateChangedEvent>(
//            [&](const GameStateChangedEvent&)
//            {
//                subscriber2++;
//            });
//
//
//
//        PrinterAdapter adapter(bus);
//
//
//        adapter.Convert(GameSnapshot{});
//
//
//        CHECK(subscriber1 == 1);
//        CHECK(subscriber2 == 1);
//    }
//
//
//
//    TEST_CASE("Adapter transfers the correct snapshot")
//    {
//        EventBus bus;
//
//
//        bool received = false;
//
//
//        bus.subscribe<GameStateChangedEvent>(
//            [&](const GameStateChangedEvent& event)
//            {
//                received = true;
//
//                const GameSnapshot& snapshot =
//                    event.getSnapshot();
//
//                // כאן להוסיף בדיקה של שדות Snapshot
//                // לדוגמה:
//                // CHECK(snapshot.getBoard() == expected);
//
//            });
//
//
//        PrinterAdapter adapter(bus);
//
//
//        GameSnapshot original;
//
//
//        adapter.Convert(original);
//
//
//        CHECK(received);
//    }
//
//
//
//    TEST_CASE("Every Convert call creates an event")
//    {
//        EventBus bus;
//
//
//        int count = 0;
//
//
//        bus.subscribe<GameStateChangedEvent>(
//            [&](const GameStateChangedEvent&)
//            {
//                count++;
//            });
//
//
//        PrinterAdapter adapter(bus);
//
//
//        adapter.Convert(GameSnapshot{});
//        adapter.Convert(GameSnapshot{});
//        adapter.Convert(GameSnapshot{});
//
//
//        CHECK(count == 3);
//    }
//
//
//
//    //TEST_CASE("Adapter subscriber exception behavior")
//    //{
//    //    EventBus bus;
//
//
//    //    bus.subscribe<GameStateChangedEvent>(
//    //        [](const GameStateChangedEvent&)
//    //        {
//    //            throw std::runtime_error(
//    //                "subscriber failed"
//    //            );
//    //        });
//
//
//    //    PrinterAdapter adapter(bus);
//
//
//
//    //    // תלוי במימוש EventBus:
//    //    // אם EventBus תופס exceptions:
//    //    CHECK_NOTHROW(
//    //        adapter.Convert(GameSnapshot{})
//    //    );
//
//    //    // אם לא:
//    //    // CHECK_THROWS(...)
//    //}
//    TEST_CASE("One subscriber failure does not stop other subscribers")
//    {
//        EventBus bus;
//
//
//        bool failedSubscriberCalled = false;
//        bool goodSubscriberCalled = false;
//
//
//        bus.subscribe<GameStateChangedEvent>(
//            [&](const GameStateChangedEvent&)
//            {
//                failedSubscriberCalled = true;
//
//                throw std::runtime_error(
//                    "WebSocket disconnected"
//                );
//            });
//
//
//        bus.subscribe<GameStateChangedEvent>(
//            [&](const GameStateChangedEvent&)
//            {
//                goodSubscriberCalled = true;
//            });
//
//
//        PrinterAdapter adapter(bus);
//
//
//        CHECK_NOTHROW(
//            adapter.Convert(GameSnapshot{})
//        );
//
//
//        CHECK(failedSubscriberCalled);
//        CHECK(goodSubscriberCalled);
//    }
//
//
//
//    TEST_CASE("Adapter can be destroyed safely")
//    {
//        EventBus bus;
//
//
//        {
//            PrinterAdapter adapter(bus);
//
//            adapter.Convert(GameSnapshot{});
//        }
//
//
//        CHECK(true);
//    }
//
//
//
//    TEST_CASE("Adapter implements IBoardPrinter interface")
//    {
//        EventBus bus;
//
//
//        PrinterAdapter adapter(bus);
//
//
//        IBoardPrinter* printer =
//            &adapter;
//
//
//        CHECK(printer != nullptr);
//    }
//
//    TEST_CASE("Subscriber receives event before unsubscribe")
//    {
//        EventBus bus;
//
//        bool received = false;
//
//        auto id = bus.subscribe<GameStateChangedEvent>(
//            [&](const GameStateChangedEvent&)
//            {
//                received = true;
//            });
//
//
//        GameSnapshot snapshot;
//
//        bus.publish(
//            GameStateChangedEvent(snapshot)
//        );
//
//
//        CHECK(received == true);
//    }
//
//
//    TEST_CASE("Subscriber does not receive event after unsubscribe")
//    {
//        EventBus bus;
//
//        bool received = false;
//
//
//        auto id = bus.subscribe<GameStateChangedEvent>(
//            [&](const GameStateChangedEvent&)
//            {
//                received = true;
//            });
//
//
//        bus.unsubscribe<GameStateChangedEvent>(id);
//
//
//        GameSnapshot snapshot;
//
//        bus.publish(
//            GameStateChangedEvent(snapshot)
//        );
//
//
//        CHECK(received == false);
//    }
//
//
//    TEST_CASE("Removing one subscriber does not affect another subscriber")
//    {
//        EventBus bus;
//
//        bool subscriberAReceived = false;
//        bool subscriberBReceived = false;
//
//
//        auto idA = bus.subscribe<GameStateChangedEvent>(
//            [&](const GameStateChangedEvent&)
//            {
//                subscriberAReceived = true;
//            });
//
//
//        auto idB = bus.subscribe<GameStateChangedEvent>(
//            [&](const GameStateChangedEvent&)
//            {
//                subscriberBReceived = true;
//            });
//
//
//        bus.unsubscribe<GameStateChangedEvent>(idA);
//
//
//        GameSnapshot snapshot;
//
//        bus.publish(
//            GameStateChangedEvent(snapshot)
//        );
//
//
//        CHECK(subscriberAReceived == false);
//        CHECK(subscriberBReceived == true);
//    }
//
//
//    TEST_CASE("Unsubscribe with non existing id does not throw")
//    {
//        EventBus bus;
//
//
//        CHECK_NOTHROW(
//            bus.unsubscribe<GameStateChangedEvent>(999999)
//        );
//    }
//
//
//    TEST_CASE("Double unsubscribe is safe")
//    {
//        EventBus bus;
//
//        bool received = false;
//
//
//        auto id = bus.subscribe<GameStateChangedEvent>(
//            [&](const GameStateChangedEvent&)
//            {
//                received = true;
//            });
//
//
//        bus.unsubscribe<GameStateChangedEvent>(id);
//
//        CHECK_NOTHROW(
//            bus.unsubscribe<GameStateChangedEvent>(id)
//        );
//
//
//        GameSnapshot snapshot;
//
//        bus.publish(
//            GameStateChangedEvent(snapshot)
//        );
//
//
//        CHECK(received == false);
//    }
//
//
//    TEST_CASE("Multiple subscriptions with same callback have different ids")
//    {
//        EventBus bus;
//
//        int receivedCount = 0;
//
//
//        auto callback =
//            [&](const GameStateChangedEvent&)
//            {
//                receivedCount++;
//            };
//
//
//        auto id1 =
//            bus.subscribe<GameStateChangedEvent>(callback);
//
//
//        auto id2 =
//            bus.subscribe<GameStateChangedEvent>(callback);
//
//
//        CHECK(id1 != id2);
//
//
//        GameSnapshot snapshot;
//
//
//        bus.publish(
//            GameStateChangedEvent(snapshot)
//        );
//
//
//        CHECK(receivedCount == 2);
//
//
//
//        bus.unsubscribe<GameStateChangedEvent>(id1);
//
//
//        receivedCount = 0;
//
//
//        bus.publish(
//            GameStateChangedEvent(snapshot)
//        );
//
//
//        CHECK(receivedCount == 1);
//    }
//
//
//    TEST_CASE("Publish only reaches active subscribers after removals")
//    {
//        EventBus bus;
//
//        bool subscriberAReceived = false;
//        bool subscriberBReceived = false;
//        bool subscriberCReceived = false;
//
//
//        auto idA = bus.subscribe<GameStateChangedEvent>(
//            [&](const GameStateChangedEvent&)
//            {
//                subscriberAReceived = true;
//            });
//
//
//        auto idB = bus.subscribe<GameStateChangedEvent>(
//            [&](const GameStateChangedEvent&)
//            {
//                subscriberBReceived = true;
//            });
//
//
//        auto idC = bus.subscribe<GameStateChangedEvent>(
//            [&](const GameStateChangedEvent&)
//            {
//                subscriberCReceived = true;
//            });
//
//
//
//        bus.unsubscribe<GameStateChangedEvent>(idA);
//        bus.unsubscribe<GameStateChangedEvent>(idC);
//
//
//
//        GameSnapshot snapshot;
//
//
//        bus.publish(
//            GameStateChangedEvent(snapshot)
//        );
//
//
//
//        CHECK(subscriberAReceived == false);
//        CHECK(subscriberBReceived == true);
//        CHECK(subscriberCReceived == false);
//    }
//    TEST_CASE("Calling unsubscribe twice does not crash")
//    {
//        EventBus bus;
//
//        bool received = false;
//
//        SubscriptionId id =
//            bus.subscribe<GameStateChangedEvent>(
//                [&](const GameStateChangedEvent&)
//                {
//                    received = true;
//                });
//
//        bus.unsubscribe<GameStateChangedEvent>(id);
//
//        CHECK_NOTHROW(
//            bus.unsubscribe<GameStateChangedEvent>(id)
//        );
//
//        GameSnapshot snapshot;
//
//        bus.publish(
//            GameStateChangedEvent(snapshot)
//        );
//
//        CHECK_FALSE(received);
//    }
//}