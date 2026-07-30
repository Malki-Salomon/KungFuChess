#include "InputHandler.h"
#include "MoveSender.h"
#include "SquareNotation.h"
#include <opencv2/highgui.hpp> // cv::namedWindow, cv::setMouseCallback

InputHandler::InputHandler(MoveSender& moveSender, const Layout& layout, const std::string& windowName,
                            IMoveIntentHint& moveIntentHint)
    : moveSender(moveSender), layout(layout), moveIntentHint(moveIntentHint)
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

    // Screen space becomes a board cell here; turning that cell into the
    // algebraic square text belongs to the protocol, and building the
    // actual message belongs to MoveSender - neither is this class's job.
    std::string square = Protocol::SquareNotation::toNotation(row, col);
    if (square.empty())
    {
        return;
    }

    // A jump names a single square and needs no second click, so it is
    // dispatched immediately and never becomes a pending origin.
    if (isDoubleClick)
    {
        moveIntentHint.hintNextMoveIsJump();
        moveSender.sendJump(square);
        pendingFromSquare.clear();
        return;
    }

    // First click picks the origin; the second one completes the move.
    // No check that the origin actually holds one of this player's pieces
    // - that is the server's call to make, and it already silently
    // rejects anything illegitimate.
    if (pendingFromSquare.empty())
    {
        pendingFromSquare = square;
        return;
    }

    // Clicking the same square twice cancels the selection rather than
    // sending a nonsensical move onto itself.
    if (pendingFromSquare == square)
    {
        pendingFromSquare.clear();
        return;
    }

    moveSender.sendMove(pendingFromSquare, square);
    pendingFromSquare.clear();
}
