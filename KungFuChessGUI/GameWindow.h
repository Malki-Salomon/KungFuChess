#pragma once
#include <string>
#include "GameObserver.h"
#include "IGameController.h"
#include "img.hpp"
#include "Layout.h"
#include "TextureManager.h"
#include "BoardRenderer.h"
#include "BoardAnimationController.h"

// Observer that owns on-screen rendering of a GameSnapshot. Holds the
// Layout/TextureManager state the renderers depend on and drives
// clear -> board -> pieces -> display each time a new snapshot arrives, or
// each time the GUI loop ticks (see tick()).
// Commands (e.g. from user input) are sent back to the Core through the
// injected IGameController, keeping the GUI decoupled from App.
//
// windowName is this GameWindow's unique display-window identity. In a
// multi-session setup (e.g. a server driving several concurrent games)
// each session's GameWindow must be constructed with its own unique name
// (a session ID works well) so their OpenCV windows don't collide - see
// Img's windowName constructor. InputHandler must be given this same
// string so its mouse callback binds to this exact window.
class GameWindow : public GameObserver
{
public:
    GameWindow(IGameController& gameController, std::string windowName);

    // Reconciles the animated pieces against the new snapshot (see
    // BoardAnimationController) and repaints.
    void update(const GameSnapshot& snapshot) override;

    // Advances animation playback by deltaMs and repaints using whatever
    // pieces are currently tracked - no snapshot needed. Call every GUI
    // loop iteration so idle/looping clips stay smooth independent of how
    // often Core pushes a new snapshot.
    void tick(long long deltaMs);

    const Layout& getLayout() const { return layout; }
    const std::string& getWindowName() const { return windowName; }

    // Exposes the animation controller's jump-hint seam to InputHandler,
    // without exposing the rest of GameWindow's rendering internals.
    IMoveIntentHint& getMoveIntentHint() { return pieceRenderer; }

private:
    // (Re)creates the backing canvas and paints the static board onto it
    // exactly once per board size, rather than every frame - this is what
    // makes BoardAnimationController's per-piece skip-if-unchanged logic
    // meaningful instead of being wiped out by a from-scratch redraw a
    // moment later.
    void ensureCanvasReady(int rows, int cols);
    void renderAndShow();

    IGameController& gameController;
    std::string windowName;
    Layout layout;
    TextureManager textureManager;
    BoardRenderer boardRenderer;
    BoardAnimationController pieceRenderer;
    Img imgWindow;
    bool canvasReady = false;
};
