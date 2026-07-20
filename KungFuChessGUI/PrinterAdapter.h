#pragma once
#include "IBoardPrinter.h"

class GameObserver;

// Adapts the Core's IBoardPrinter output-device interface to the GUI's
// Observer pattern, so App can keep calling Convert() without knowing
// anything about GameWindow or GameObserver.
class PrinterAdapter : public IBoardPrinter
{
public:
    explicit PrinterAdapter(GameObserver& observer);

    void Convert(const GameSnapshot& snapshot) override;

private:
    GameObserver& observer;
};
