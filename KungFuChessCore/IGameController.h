#pragma once
#include <memory>
#include <string>
#include <vector>

class IBoardPrinter;

class IGameController {
public:
    virtual ~IGameController() = default;
    virtual void dispatchCommand(const std::string& cmd) = 0;
    virtual void setOutputDevice(IBoardPrinter* printer) = 0;
    virtual void parseLoad(const std::vector<std::string>& inputLines) = 0;
    virtual void run() = 0;
};

namespace CoreFactory
{
    std::unique_ptr<IGameController> createGameController();
}
