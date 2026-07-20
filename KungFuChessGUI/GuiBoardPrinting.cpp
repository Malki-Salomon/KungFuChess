#include "GuiBoardPrinting.h"
#include "Game.h"
#include "GameSnapshot.h"
#include "SnapshotBoardConvert.h"
#include <stdexcept>

GuiBoardPrinting::GuiBoardPrinting()
    : boardRenderer(textureManager, layout, R"(.\pictures\board_classic.png)")
    , pieceRenderer(textureManager, layout)
{
}

void GuiBoardPrinting::Convert(const GameSnapshot& snapshot)
{
    int rows = static_cast<int>(snapshot.cells.size());
    int cols = rows > 0 ? static_cast<int>(snapshot.cells[0].size()) : 0;

    layout.setRows(rows);
    layout.setCols(cols);

    cv::Rect boardRect = layout.getBoardRect();
    boardImg.create(boardRect.width, boardRect.height);
    boardRenderer.draw(boardImg);
    pieceRenderer.draw(boardImg, snapshot);

	boardImg.show();
}
