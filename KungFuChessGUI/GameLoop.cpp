#include "GameLoop.h"
#include <opencv2/highgui.hpp>
#include <chrono>
constexpr int FRAME_DELAY_MS = 16;
constexpr int STOP = 27;

GameLoop::GameLoop(GameWindow& gameWindow)
    : gameWindow(gameWindow)
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

        // Advances animation playback every loop iteration, independent of
        // how often the server pushes a new snapshot, so idle/looping
        // sprite clips stay smooth on frames where nothing changed.
        gameWindow.tick(deltaTimeMs);

        int key = cv::waitKey(FRAME_DELAY_MS);
        if (key == STOP) {
            running = false;
        }
    }
}
