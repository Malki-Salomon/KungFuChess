#pragma once
#include <algorithm>
#include <cstddef>
#include <functional>
#include <map>
#include <typeindex>
#include <vector>
#include "Event.h"

// Opaque handle returned by subscribe() and later passed to unsubscribe().
// Unique across the whole EventBus (not just within one event type), so a
// caller never needs to remember which TEvent an id belongs to beyond what
// unsubscribe<TEvent>() itself already requires.
using SubscriptionId = size_t;

// Generic publish/subscribe hub. Knows nothing about Game, Board, or the
// GUI - subscribers register for whichever concrete Event subclass they
// care about (as a template argument), and publish() fans a published
// event out to every callback registered for that exact type. This is the
// only mechanism Core uses to notify external listeners of something
// happening, without Core ever holding a reference to (or even knowing the
// existence of) any specific listener - any number of subscribers, of any
// event type, can be added (or removed, via unsubscribe) without EventBus
// or its publishers changing.
//
// subscribe/publish/unsubscribe are templates and must stay defined here in
// the header (a template's body has to be visible at each instantiation
// site); there is no non-template logic left over for EventBus.cpp to hold.
class EventBus
{
public:
    template<typename TEvent>
    SubscriptionId subscribe(std::function<void(const TEvent&)> callback)
    {
        SubscriptionId id = nextSubscriptionId++;

        auto typeErasedCallback = [callback](const Event& event)
        {
            callback(static_cast<const TEvent&>(event));
        };

        subscribersByType[std::type_index(typeid(TEvent))].push_back({ id, std::move(typeErasedCallback) });

        return id;
    }

    // Removes the single subscriber previously returned by subscribe<TEvent>()
    // as `id`. A no-op (not an error) if `id` doesn't match any current
    // subscriber for TEvent - already unsubscribed, or never existed - so
    // callers don't need to track whether they're "still" subscribed.
    // Never affects subscribers of any other id, or of any other event type.
    template<typename TEvent>
    void unsubscribe(SubscriptionId id)
    {
        auto it = subscribersByType.find(std::type_index(typeid(TEvent)));
        if (it == subscribersByType.end())
            return;

        auto& subscribers = it->second;
        subscribers.erase(
            std::remove_if(subscribers.begin(), subscribers.end(),
                [id](const Subscriber& subscriber) { return subscriber.id == id; }),
            subscribers.end());
    }

    template<typename TEvent>
    void publish(const TEvent& event)
    {
        auto it = subscribersByType.find(std::type_index(typeid(TEvent)));
        if (it == subscribersByType.end())
            return;

        // Dispatched from a copy of the subscriber list: if a callback
        // reacts to this event by unsubscribing (itself or another
        // subscriber of the same type) - e.g. a component tearing itself
        // down in response to the very event being delivered - that must
        // not invalidate the list this loop is iterating.
        std::vector<Subscriber> subscribersSnapshot = it->second;

        for (const auto& subscriber : subscribersSnapshot)
        {
            try
            {
                subscriber.callback(event);
            }
            catch (...)
            {
                // A failing subscriber must not stop the event from
                // reaching the rest (e.g. a future WebSocket/Logger
                // subscriber shouldn't be starved by an unrelated one
                // throwing). TODO: route this to a future Logger instead of
                // silently swallowing it.
            }
        }
    }

private:
    struct Subscriber
    {
        SubscriptionId id;
        std::function<void(const Event&)> callback;
    };

    std::map<std::type_index, std::vector<Subscriber>> subscribersByType;
    SubscriptionId nextSubscriptionId = 1;
};
