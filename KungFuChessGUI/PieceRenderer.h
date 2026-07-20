#pragma once
#include "img.hpp"

class TextureManager;
class Layout;
struct GameSnapshot;

// Sole owner of the logic for drawing chess pieces onto the board: walks the
// snapshot's cells, asks Layout for each cell's drawing rect, and TextureManager
// for the (cached) piece texture.
class PieceRenderer
{
public:
    PieceRenderer(TextureManager& textureManager, const Layout& layout);

    void draw(Img& canvas, const GameSnapshot& snapshot) const;

private:
    TextureManager& textureManager;
    const Layout& layout;
};
