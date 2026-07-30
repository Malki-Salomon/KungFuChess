#pragma once
#include <mutex>
#include <string>
#include "GameSnapshot.h"
#include "img.hpp"
#include "Layout.h"
#include "TextureManager.h"
#include "BoardRenderer.h"
#include "BoardAnimationController.h"
#include "Events/EventBus.h"

// Owns on-screen rendering of a GameSnapshot. Holds the Layout/TextureManager
// state the renderers depend on and drives clear -> board -> pieces ->
// display, once per GUI loop tick (see tick()).
//
// Purely a display: it renders whatever snapshot it is handed and has no
// way to affect the game at all. Outgoing user input travels an entirely
// separate path (InputHandler -> MoveSender -> the server), so this class
// no longer holds any game controller - there is no local game to hold.
//
// THREADING: snapshots now arrive from the network, which means update() is
// called on WebSocketClient's receive thread while the main thread is
// pumping cv::waitKey(). OpenCV's HighGUI is not safe to drive from two
// threads, so update() deliberately does NO rendering - it only parks the
// snapshot - and tick() (main thread) is what actually paints it. Painting
// straight from update() on the receive thread is exactly what crashed
// before this split existed.
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
    GameWindow(std::string windowName, EventBus& eventBus);

    // Unsubscribes from eventBus so a destroyed GameWindow can never be
    // called back into - without this, publish() would still hold (and
    // eventually invoke) a callback capturing a dangling `this`.
    ~GameWindow();

    GameWindow(const GameWindow&) = delete;
    GameWindow& operator=(const GameWindow&) = delete;

    // Accepts a new snapshot to display. Called from the network receive
    // thread (via the EventBus subscription), so it must stay cheap and
    // must not touch OpenCV - it only stores the snapshot for the next
    // tick() to pick up. Superseding an unapplied snapshot is intentional:
    // only the newest board state is worth drawing.
    void update(const GameSnapshot& snapshot);

    // Applies whatever snapshot arrived since the last call (if any),
    // advances animation playback by deltaMs, and repaints. Must be called
    // from the thread that owns the window (the one pumping cv::waitKey) -
    // this is where all rendering actually happens.
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

    // Reconciles the animated pieces against a snapshot and prepares the
    // canvas for it. Main thread only, called from tick().
    void applySnapshot(const GameSnapshot& snapshot);

    std::string windowName;
    Layout layout;
    TextureManager textureManager;
    BoardRenderer boardRenderer;
    BoardAnimationController pieceRenderer;
    Img imgWindow;
    bool canvasReady = false;

    // Hand-off between the network receive thread (writer, via update())
    // and the main/render thread (reader, via tick()).
    std::mutex pendingSnapshotMutex;
    GameSnapshot pendingSnapshot;
    bool hasPendingSnapshot = false;

    EventBus& eventBus;
    SubscriptionId subscriptionId;
};
