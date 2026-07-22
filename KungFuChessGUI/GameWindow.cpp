#include "GameWindow.h"
#include "GameSnapshot.h"

GameWindow::GameWindow(IGameController& gameController, std::string windowName)
    : gameController(gameController)
    , windowName(windowName)
    , boardRenderer(textureManager, layout, R"(.\pictures\board_classic.png)")
    , pieceRenderer(textureManager, layout)
    , imgWindow(windowName)
{
}

void GameWindow::update(const GameSnapshot& snapshot)
{
    int rows = static_cast<int>(snapshot.cells.size());
    int cols = rows > 0 ? static_cast<int>(snapshot.cells[0].size()) : 0;

    ensureCanvasReady(rows, cols);

    pieceRenderer.onSnapshotReceived(snapshot);

    renderAndShow();
}

void GameWindow::tick(long long deltaMs)
{
    pieceRenderer.update(deltaMs);
    renderAndShow();
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
