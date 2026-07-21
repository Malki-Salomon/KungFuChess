#include "GameLoop.h"
#include <opencv2/highgui.hpp>

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

        // אם עבר זמן (למשל 10 או 16 מילישניות), נעדכן את ה-Core כמה זמן עבר
        if (deltaTimeMs > 0)
        {
            // יוצרים פקודת Wait עם פרק הזמן האמיתי שנמדד בעזרת chrono
            /*Command waitCmd;
            waitCmd.type = CommandType::Wait;
            waitCmd.ms = deltaTimeMs;*/
			gameController.dispatchCommand("wait " + deltaTimeMs);

            // מכניסים את הפקודה לתור של ה-Game (מתוך קוד הניהול או בקר המשחק)
            // gameController.addCommand(waitCmd); // תלוי במבנה הנתונים אצלך
        }
        gameController.run();

        int key = cv::waitKey(10);
        if (key == 27) { // ��� Esc
            running = false;
        }
    }
}