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

    layout.setRows(rows);
    layout.setCols(cols);

    cv::Rect boardRect = layout.getBoardRect();

    imgWindow.create(boardRect.width, boardRect.height);

    boardRenderer.draw(imgWindow);
    pieceRenderer.draw(imgWindow, snapshot);

    imgWindow.show();
}
