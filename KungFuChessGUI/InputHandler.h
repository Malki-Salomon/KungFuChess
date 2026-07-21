#pragma once
#include <string>
#include "IGameController.h"
#include "Layout.h"

// Owns all mouse input for the game: registers itself as the display
// window's mouse-event sink and translates raw clicks into board
// coordinates and commands. This is the only class that touches the
// display library's mouse-callback API - GameLoop and GameWindow don't
// need to know it exists.
class InputHandler
{
public:
    // windowName must match the window the game is rendered into (see
    // Img::kWindowName); the handler attaches itself to it immediately.
    InputHandler(IGameController& gameController, const Layout& layout, const std::string& windowName);

    void handleMouseClick(int pixelX, int pixelY, bool isDoubleClick);

private:
    // Registers this instance as the window's mouse-event sink.
    void attachToWindow(const std::string& windowName);

    // The display library requires a plain function pointer for its mouse
    // callback, so this trampoline forwards the event to the owning
    // InputHandler instance (passed through as userdata) and on to
    // handleMouseClick, which remains the only place that interprets clicks.
    static void onMouseEvent(int event, int x, int y, int flags, void* userdata);

    IGameController& gameController;
    const Layout& layout;
};