#pragma once

// Polymorphic base for every event published through EventBus. Deliberately
// carries no data or behavior of its own - EventBus handles events only in
// terms of this type, type-erased down to "some event happened"; the real
// payload lives on the concrete subclass (see GameStateChangedEvent) and is
// recovered via the TEvent template argument each subscriber registered
// with.
class Event
{
public:
    virtual ~Event() = default;
};
