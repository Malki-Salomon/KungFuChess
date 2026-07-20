#include "PieceRenderer.h"
#include "TextureManager.h"
#include "Layout.h"
#include "GameSnapshot.h"
#include "PieceRegistry.h"

PieceRenderer::PieceRenderer(TextureManager& textureManager, const Layout& layout)
    : textureManager(textureManager), layout(layout)
{
}

void PieceRenderer::draw(Img& canvas, const GameSnapshot& snapshot) const
{
    int cellSize = layout.getCellSize();
    int rows = static_cast<int>(snapshot.cells.size());

    for (int row = 0; row < rows; ++row)
    {
        int cols = static_cast<int>(snapshot.cells[row].size());
        for (int col = 0; col < cols; ++col)
        {
            const PieceSnapshot& cell = snapshot.cells[row][col];
            if (cell.type == PieceType::Empty)
                continue;

            const PieceMetadata& metadata = PieceRegistry::getMetadata(cell.type);
            const std::string& imagePath = (cell.color == PieceColor::White)
                ? metadata.whiteImagePath
                : metadata.blackImagePath;

            cv::Rect cellRect = layout.getCellRect(row, col);

            // Local copy: getTexture returns a const reference into the cache,
            // and draw_on is declared non-const on its source.
            Img pieceImg = textureManager.getTexture(imagePath, { cellSize, cellSize }, true);
            pieceImg.draw_on(canvas, cellRect.x, cellRect.y);
        }
    }
}
