#include "PrinterAdapter.h"
#include "GameObserver.h"

PrinterAdapter::PrinterAdapter(GameObserver& observer)
    : observer(observer)
{
}

void PrinterAdapter::Convert(const GameSnapshot& snapshot)
{
    observer.update(snapshot);
}
