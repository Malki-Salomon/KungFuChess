#pragma once
#include <string>
#include "Layout.h"
#include "IMoveIntentHint.h"

class MoveSender;

// Owns all mouse input for the game: registers itself as the display
// window's mouse-event sink and translates raw clicks into board
// coordinates and outgoing move/jump requests. This is the only class that
// touches the display library's mouse-callback API - GameLoop and
// GameWindow don't need to know it exists.
//
// Deliberately knows nothing about the wire protocol: it names squares and
// calls MoveSender, which owns building and sending the actual messages.
// It also performs NO legality or ownership checking - the server is the
// sole authority on whether a requested move is allowed, and silently
// rejects anything illegitimate (see the Level 5 directive). A first click
// selects an origin square, a second one requests the move.
class InputHandler
{
public:
    // windowName must match the window the game is rendered into (see
    // Img::kWindowName); the handler attaches itself to it immediately.
    // moveIntentHint lets a dispatched jump be flagged to the animation
    // system (see IMoveIntentHint) - purely a GUI-to-GUI hint, the server
    // is never involved in it.
    InputHandler(MoveSender& moveSender, const Layout& layout, const std::string& windowName,
                 IMoveIntentHint& moveIntentHint);

    void handleMouseClick(int pixelX, int pixelY, bool isDoubleClick);

private:
    // Registers this instance as the window's mouse-event sink.
    void attachToWindow(const std::string& windowName);

    // The display library requires a plain function pointer for its mouse
    // callback, so this trampoline forwards the event to the owning
    // InputHandler instance (passed through as userdata) and on to
    // handleMouseClick, which remains the only place that interprets clicks.
    static void onMouseEvent(int event, int x, int y, int flags, void* userdata);

    MoveSender& moveSender;
    const Layout& layout;
    IMoveIntentHint& moveIntentHint;

    // The origin square of a move being composed, empty when no square is
    // selected yet. Purely local click-pairing state - it is NOT a claim
    // about what the server considers selected, and nothing is drawn from
    // it; the board on screen only ever reflects server snapshots.
    std::string pendingFromSquare;
};
