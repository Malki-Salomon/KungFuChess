#pragma once

#include <functional>
#include <string>
#include <vector>

class EventBus;

// Translates incoming protocol messages into Core types and publishes them
// on the EventBus. This is the GUI's entire inbound networking seam: it is
// the only place wire text becomes a GameSnapshot.
//
// Deliberately narrow responsibilities (see the Level 5 directive):
//  - It never touches WebSocketClient (it's handed already-received text)
//    and never holds a GameWindow - only an EventBus, so rendering stays
//    genuinely decoupled from networking rather than decoupled by
//    convention.
//  - It never infers game state the server didn't send. No legality,
//    ownership, turn or check logic lives here.
//  - It never publishes a partially-parsed board (see handleBoard).
//
// New message types are added by registering another handler in
// registerHandlers(), not by editing the dispatch of existing ones.
class NetworkSnapshotAdapter
{
public:
    explicit NetworkSnapshotAdapter(EventBus& eventBus);

    // Feed one raw message, exactly as received. Safe to call from
    // WebSocketClient's receive thread. Unrecognized messages are ignored.
    void onMessage(const std::string& rawMessage);

private:
    // Each handler tries to parse rawMessage as its own message type and
    // returns whether it did - the first one that claims the message wins.
    // Adding a message type means appending one of these, leaving every
    // existing handler untouched.
    using MessageHandler = std::function<bool(const std::string&)>;

    void registerHandlers();

    // Rebuilds a full GameSnapshot from the message's symbol grid and
    // publishes a GameStateChangedEvent. If ANY cell carries a symbol
    // PieceRegistry doesn't recognize, the whole snapshot is discarded and
    // nothing is published - a bad message must read as if it never
    // arrived, never as a half-updated board.
    bool handleBoard(const std::string& rawMessage);
    bool handleGameOver(const std::string& rawMessage);
    bool handleAssigned(const std::string& rawMessage);
    bool handleOpponentDisconnected(const std::string& rawMessage);
    bool handleOpponentReconnected(const std::string& rawMessage);

    EventBus& eventBus;
    std::vector<MessageHandler> handlers;
};
