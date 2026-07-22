#pragma once
#include <string>
#include "img.hpp"

class TextureManager;
class Layout;

// Sole owner of the logic for drawing the chess board background:
// asks Layout for the board's drawing area and TextureManager for the
// (cached) board texture, then composites it onto the given canvas.
class BoardRenderer
{
public:
    BoardRenderer(TextureManager& textureManager, const Layout& layout, const std::string& texturePath);

    void draw(Img& canvas) const;

    // The board's cached background texture, sized to the current board
    // rect (the same cached Img draw() uses). Exposed so other renderers
    // (BoardAnimationController) can repaint a small patch of "just the
    // board" to erase a piece sprite drawn on top of it earlier, without
    // needing to know the texture path themselves or redraw the whole
    // canvas.
    const Img& getTexture() const;

private:
    TextureManager& textureManager;
    const Layout& layout;
    std::string texturePath;
};
