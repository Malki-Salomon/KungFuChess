#pragma once
#include "GameObserver.h"
#include "IGameController.h"
#include "img.hpp"
#include "Layout.h"
#include "TextureManager.h"
#include "BoardRenderer.h"
#include "PieceRenderer.h"

// Observer that owns on-screen rendering of a GameSnapshot. Holds the
// Layout/TextureManager state the renderers depend on and drives
// clear -> board -> pieces -> display each time a new snapshot arrives.
// Commands (e.g. from user input) are sent back to the Core through the
// injected IGameController, keeping the GUI decoupled from App.
class GameWindow : public GameObserver
{
public:
    explicit GameWindow(IGameController& gameController);

    void update(const GameSnapshot& snapshot) override;

private:
    IGameController& gameController;
    Layout layout;
    TextureManager textureManager;
    BoardRenderer boardRenderer;
    PieceRenderer pieceRenderer;
    Img imgWindow;
};
