#pragma once
#include <string>
#include "IGameController.h"
#include "img.hpp"
#include "Layout.h"
#include "TextureManager.h"
#include "BoardRenderer.h"
#include "BoardAnimationController.h"
#include "Events/EventBus.h"

// Owns on-screen rendering of a GameSnapshot. Holds the Layout/TextureManager
// state the renderers depend on and drives clear -> board -> pieces ->
// display each time a new snapshot arrives (via its EventBus subscription),
// or each time the GUI loop ticks (see tick()).
// Commands (e.g. from user input) are sent back to the Core through the
// injected IGameController, keeping the GUI decoupled from App.
//
// windowName is this GameWindow's unique display-window identity. In a
// multi-session setup (e.g. a server driving several concurrent games)
// each session's GameWindow must be constructed with its own unique name
// (a session ID works well) so their OpenCV windows don't collide - see
// Img's windowName constructor. InputHandler must be given this same
// string so its mouse callback binds to this exact window.
class GameWindow
{
public:
    // Subscribes to GameStateChangedEvent on eventBus immediately, so this
    // instance starts reacting to state changes as soon as it exists - no
    // separate wiring step (like the old setObserver()/PrinterAdapter(this)
    // pairing) is needed at the call site.
    GameWindow(IGameController& gameController, std::string windowName, EventBus& eventBus);

    // Unsubscribes from eventBus so a destroyed GameWindow can never be
    // called back into - without this, publish() would still hold (and
    // eventually invoke) a callback capturing a dangling `this`.
    ~GameWindow();

    GameWindow(const GameWindow&) = delete;
    GameWindow& operator=(const GameWindow&) = delete;

    // Reconciles the animated pieces against the new snapshot (see
    // BoardAnimationController) and repaints. No longer a virtual override -
    // called directly from the EventBus subscription lambda instead of
    // through a GameObserver base class.
    void update(const GameSnapshot& snapshot);

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

    EventBus& eventBus;
    SubscriptionId subscriptionId;
};
