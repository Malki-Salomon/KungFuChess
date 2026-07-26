#include "PrinterAdapter.h"
#include "Events/EventBus.h"
#include "Events/GameStateChangedEvent.h"

PrinterAdapter::PrinterAdapter(EventBus& eventBus)
    : eventBus(eventBus)
{
}

void PrinterAdapter::Convert(const GameSnapshot& snapshot)
{
    eventBus.publish(GameStateChangedEvent(snapshot));
}
