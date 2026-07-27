#pragma once

#include <memory>
#include <string>

class IGameController;
class IBoardPrinter;

// Manages a single running game. Does NOT know how the game was created
// (starting position / save / replay) - it only receives an already
// initialized controller and drives it forward.
class GameSession
{
public:
    explicit GameSession(std::unique_ptr<IGameController> controller);
    ~GameSession(); // declared here, defined in the .cpp where IGameController is complete

    void tick();

    // Forwards a raw text command (e.g. "click 3 4") straight to the
    // controller, exactly the same format the GUI's InputHandler already
    // produces. Must only be called from the game's own tick thread - see
    // CommandInbox for how the Network layer hands commands off safely.
    void dispatchCommand(const std::string& cmd);

    // Attaches an IBoardPrinter that will receive a callback every time the
    // board changes (see Game::publishSnapshotIfReady / processNextCommand).
    void attachPrinter(IBoardPrinter* printer);

private:
    std::unique_ptr<IGameController> m_controller;
};

