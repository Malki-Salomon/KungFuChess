#include "GuiBoardPrinting.h"
#include "BoardMapper.h"
#include "Game.h"
#include "GameSnapshot.h"
#include "PieceRegistry.h"
#include "SnapshotBoardConvert.h"
#include <stdexcept>

void GuiBoardPrinting::Convert(const GameSnapshot& snapshot)
{
    int rows = static_cast<int>(snapshot.cells.size());
    int cols = rows > 0 ? static_cast<int>(snapshot.cells[0].size()) : 0;

    boardImg.read(R"(.\pictures\board_classic.png)", { cols * PIXEL, rows * PIXEL }, false);

    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            const PieceSnapshot& cell = snapshot.cells[row][col];
            if (cell.type == PieceType::Empty)
                continue;

            const PieceMetadata& metadata = PieceRegistry::getMetadata(cell.type);
            const std::string& imagePath = (cell.color == PieceColor::White)
                ? metadata.whiteImagePath
                : metadata.blackImagePath;

            Img pieceImg;
            pieceImg.read(imagePath, { PIXEL, PIXEL }, true);
            pieceImg.draw_on(boardImg, col * PIXEL, row * PIXEL);
        }
    }
	boardImg.show();
}
