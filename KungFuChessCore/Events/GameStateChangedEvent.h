#pragma once
#include "Event.h"
#include "GameSnapshot.h"

// Published whenever the board state changes and consumers (the GUI today;
// potentially a WebSocket relay or logger later) need to know the new
// state. Replaces the old GameObserver::update(snapshot) call - carries an
// owned copy of the snapshot (not a reference) since publish() is commonly
// given a temporary, and a subscriber may reasonably want to hold onto the
// event past the call that published it.
class GameStateChangedEvent : public Event
{
public:
    explicit GameStateChangedEvent(const GameSnapshot& snapshot);

    const GameSnapshot& getSnapshot() const;

private:
    GameSnapshot snapshot;
};
