#include "GameWindow.h"
#include "GameSnapshot.h"
#include "Events/GameStateChangedEvent.h"

GameWindow::GameWindow(std::string windowName, EventBus& eventBus)
    : windowName(windowName)
    , boardRenderer(textureManager, layout, R"(.\pictures\board_classic.png)")
    , pieceRenderer(textureManager, layout)
    , imgWindow(windowName)
    , eventBus(eventBus)
{
    subscriptionId = eventBus.subscribe<GameStateChangedEvent>(
        [this](const GameStateChangedEvent& event)
        {
            update(event.getSnapshot());
        });
}

GameWindow::~GameWindow()
{
    eventBus.unsubscribe<GameStateChangedEvent>(subscriptionId);
}

void GameWindow::update(const GameSnapshot& snapshot)
{
    // Receive-thread side of the hand-off: park the snapshot and return.
    // Deliberately no rendering here - see the THREADING note in the header.
    std::lock_guard<std::mutex> lock(pendingSnapshotMutex);
    pendingSnapshot = snapshot;
    hasPendingSnapshot = true;
}

void GameWindow::tick(long long deltaMs)
{
    // Main-thread side of the hand-off. The snapshot is copied out under
    // the lock and applied outside it, so the receive thread is never held
    // up by the (comparatively slow) texture loading and painting work.
    GameSnapshot snapshotToApply;
    bool shouldApply = false;
    {
        std::lock_guard<std::mutex> lock(pendingSnapshotMutex);
        if (hasPendingSnapshot)
        {
            snapshotToApply = std::move(pendingSnapshot);
            hasPendingSnapshot = false;
            shouldApply = true;
        }
    }

    if (shouldApply)
        applySnapshot(snapshotToApply);

    // Nothing has been received yet, so there is no canvas to paint on -
    // the board size itself comes from the first snapshot. Ticks before
    // then are a no-op rather than an attempt to show an empty image.
    if (!canvasReady)
        return;

    pieceRenderer.update(deltaMs);
    renderAndShow();
}

void GameWindow::applySnapshot(const GameSnapshot& snapshot)
{
    int rows = static_cast<int>(snapshot.cells.size());
    int cols = rows > 0 ? static_cast<int>(snapshot.cells[0].size()) : 0;

    ensureCanvasReady(rows, cols);

    pieceRenderer.onSnapshotReceived(snapshot);
}

void GameWindow::ensureCanvasReady(int rows, int cols)
{
    bool layoutChanged = rows != layout.getRows() || cols != layout.getCols();
    layout.setRows(rows);
    layout.setCols(cols);

    if (canvasReady && !layoutChanged)
        return;

    cv::Rect boardRect = layout.getBoardRect();
    imgWindow.create(boardRect.width, boardRect.height);
    boardRenderer.draw(imgWindow);

    // The fresh canvas has no pieces painted on it yet, so any piece's
    // "already drawn here" bookkeeping from a previous canvas is now stale.
    pieceRenderer.forceFullRedraw();

    canvasReady = true;
}

void GameWindow::renderAndShow()
{
    pieceRenderer.draw(imgWindow, boardRenderer.getTexture());
    imgWindow.show();
}
