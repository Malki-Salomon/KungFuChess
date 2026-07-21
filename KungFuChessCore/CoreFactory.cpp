#include "pch.h"
#include "IGameController.h"
#include "App.h"

namespace CoreFactory
{
    std::unique_ptr<IGameController> createGameController()
    {
        return std::make_unique<App>();
    }
}
