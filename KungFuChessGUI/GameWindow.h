#pragma once
#include <string>
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
//
// windowName is this GameWindow's unique display-window identity. In a
// multi-session setup (e.g. a server driving several concurrent games)
// each session's GameWindow must be constructed with its own unique name
// (a session ID works well) so their OpenCV windows don't collide - see
// Img's windowName constructor. InputHandler must be given this same
// string so its mouse callback binds to this exact window.
class GameWindow : public GameObserver
{
public:
    GameWindow(IGameController& gameController, std::string windowName);

    void update(const GameSnapshot& snapshot) override;

    const Layout& getLayout() const { return layout; }
    const std::string& getWindowName() const { return windowName; }

private:
    IGameController& gameController;
    std::string windowName;
    Layout layout;
    TextureManager textureManager;
    BoardRenderer boardRenderer;
    PieceRenderer pieceRenderer;
    Img imgWindow;
};
