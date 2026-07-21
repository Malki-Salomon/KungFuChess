#include "InputHandler.h"
#include <opencv2/highgui.hpp> // cv::namedWindow, cv::setMouseCallback

InputHandler::InputHandler(IGameController& gameController, const Layout& layout, const std::string& windowName)
    : gameController(gameController), layout(layout)
{
    attachToWindow(windowName);
}

void InputHandler::attachToWindow(const std::string& windowName)
{
    // Make sure the window exists before binding a callback to it, and pass
    // `this` through as userdata so the static trampoline can reach back
    // into this instance.
    cv::namedWindow(windowName);
    cv::setMouseCallback(windowName, &InputHandler::onMouseEvent, this);
}

void InputHandler::onMouseEvent(int event, int x, int y, int /*flags*/, void* userdata)
{
    InputHandler* self = static_cast<InputHandler*>(userdata);

    if (event == cv::EVENT_RBUTTONDOWN) {
        self->handleMouseClick(x, y, true);
    }
    else if (event == cv::EVENT_LBUTTONDOWN) {
        self->handleMouseClick(x, y, false);
    }
}

void InputHandler::handleMouseClick(int pixelX, int pixelY, bool isDoubleClick)
{
    auto offset = layout.getBoardOffset();
    int cellSize = layout.getCellSize();

    int col = (pixelX - offset.x) / cellSize;
    int row = (pixelY - offset.y) / cellSize;

    if (col >= 0 && col < layout.getCols() && row >= 0 && row < layout.getRows())
    {
        std::string command;
        if (isDoubleClick) {
            command = "jump " + std::to_string(col) + " " + std::to_string(row);
        }
        else {
            command = "click " + std::to_string(col) + " " + std::to_string(row);
        }

        gameController.dispatchCommand(command);
    }
}