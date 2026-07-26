#pragma once
#include "IBoardPrinter.h"

class EventBus;

// Adapts the Core's IBoardPrinter output-device interface to the EventBus:
// publishes a GameStateChangedEvent for each snapshot Board::print() hands
// it, so any number of subscribers (GameWindow today; a future WebSocket
// relay, logger, etc.) can react without App or Game knowing any of them
// exist. Replaces the old GameObserver-forwarding version of this class.
class PrinterAdapter : public IBoardPrinter
{
public:
    explicit PrinterAdapter(EventBus& eventBus);

    void Convert(const GameSnapshot& snapshot) override;

private:
    EventBus& eventBus;
};
