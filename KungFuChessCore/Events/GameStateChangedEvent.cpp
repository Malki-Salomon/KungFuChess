#include "pch.h"
#include "GameStateChangedEvent.h"

GameStateChangedEvent::GameStateChangedEvent(const GameSnapshot& snapshot)
    : snapshot(snapshot)
{
}

const GameSnapshot& GameStateChangedEvent::getSnapshot() const
{
    return snapshot;
}
