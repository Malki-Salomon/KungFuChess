Wire protocol layer.

Current state: a narrow, purpose-built protocol (see MiniJson.h for why it's
not a general JSON parser):
  - Client -> Server: {"type":"move","from":"e2","to":"e4"} (algebraic
    squares) - MoveTranslator converts this into the two pixel-based
    "click X Y" commands Core's StringCommandConvert actually understands.
  - Server -> Client: {"type":"board","cells":[["bR","bN",...],...]} - built
    directly in Network/SnapshotBroadcaster.cpp.

Not yet included: player identity/turn ownership, error responses for
illegal moves (Core's Controller::executeClick silently no-ops on an
illegal move - the client currently just sees nothing happen), login,
matchmaking, or room messages. Those are separate, larger pieces of work
building on top of this.
