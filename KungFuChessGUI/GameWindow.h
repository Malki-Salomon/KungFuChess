#pragma once
#include "GameObserver.h"
#include "img.hpp"
#include "Layout.h"
#include "TextureManager.h"
#include "BoardRenderer.h"
#include "PieceRenderer.h"

// Observer that owns on-screen rendering of a GameSnapshot. Holds the
// Layout/TextureManager state the renderers depend on and drives
// clear -> board -> pieces -> display each time a new snapshot arrives.
class GameWindow : public GameObserver
{
public:
    GameWindow();

    void update(const GameSnapshot& snapshot) override;

private:
    Layout layout;
    TextureManager textureManager;
    BoardRenderer boardRenderer;
    PieceRenderer pieceRenderer;
    Img imgWindow;
};
