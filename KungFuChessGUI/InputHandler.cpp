#include "InputHandler.h"
#include <opencv2/highgui.hpp> // cv::namedWindow, cv::setMouseCallback

InputHandler::InputHandler(IGameController& gameController, const Layout& layout, const std::string& windowName,
                            IMoveIntentHint& moveIntentHint)
    : gameController(gameController), layout(layout), moveIntentHint(moveIntentHint)
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
    cv::Point cell = layout.pixelToCell(cv::Point(pixelX, pixelY));
    int col = cell.x;
    int row = cell.y;

    if (col < 0 || col >= layout.getCols() || row < 0 || row >= layout.getRows())
    {
        return;
    }

    // Core now receives board cells directly, not pixels - this is the only
    // place that translates screen space into the "click col row" /
    // "jump col row" protocol Core parses (see StringCommandConvert).
    std::string command;
    if (isDoubleClick) {
        moveIntentHint.hintNextMoveIsJump();
        command = "jump " + std::to_string(col) + " " + std::to_string(row);
    }
    else {
        command = "click " + std::to_string(col) + " " + std::to_string(row);
    }

    gameController.dispatchCommand(command);
}