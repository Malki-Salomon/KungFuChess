#include "GameLoop.h"
#include <opencv2/highgui.hpp>
constexpr int FRAME_DELAY_MS = 16; 
constexpr int STOP = 27; 

GameLoop::GameLoop(IGameController& gameController, GameWindow& gameWindow)
    : gameController(gameController), gameWindow(gameWindow)
{
}

void GameLoop::run()
{
    bool running = true;
    auto prevTime = std::chrono::steady_clock::now();
    while (running)
    {

        auto currentTime = std::chrono::steady_clock::now();
        auto deltaTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - prevTime).count();
        prevTime = currentTime;

        if (deltaTimeMs > 0)
        {
 
			gameController.dispatchCommand("wait " + std::to_string(deltaTimeMs));

        }
        gameController.run();

        int key = cv::waitKey(FRAME_DELAY_MS);
        if (key == STOP) {
            running = false;
        }
    }
}