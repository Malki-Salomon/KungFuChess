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

private:
    TextureManager& textureManager;
    const Layout& layout;
    std::string texturePath;
};
