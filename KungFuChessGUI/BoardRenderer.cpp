#include "BoardRenderer.h"
#include "TextureManager.h"
#include "Layout.h"

BoardRenderer::BoardRenderer(TextureManager& textureManager, const Layout& layout, const std::string& texturePath)
    : textureManager(textureManager), layout(layout), texturePath(texturePath)
{
}

void BoardRenderer::draw(Img& canvas) const
{
    cv::Rect boardRect = layout.getBoardRect();

    // getTexture returns a const reference into the cache; draw_on is declared
    // non-const on its source, so we draw from a shallow copy rather than
    // modifying Img's interface.
    Img boardTexture = textureManager.getTexture(texturePath, { boardRect.width, boardRect.height });
    boardTexture.draw_on(canvas, boardRect.x, boardRect.y);
}
