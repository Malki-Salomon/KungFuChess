Wire protocol layer - knows about both the game (Core types) and how to
reach clients (calls into Network/WebSocketServer), which is exactly why
it's its own layer rather than living in either.

Current state: a narrow, purpose-built protocol (see MiniJson.h for why
it's not a general JSON parser):
  - Client -> Server: {"type":"move","from":"e2","to":"e4"} (algebraic
    squares) - MoveTranslator converts this into the two board-coordinate
    "click X Y" commands Core's StringCommandConvert actually understands.
  - Server -> Client: {"type":"board","cells":[["bR","bN",...],...]} -
    built in SnapshotBroadcaster.cpp. That's its only job (broadcasting) -
    it does not cache board state. When Server needs to know what the
    board currently looks like (e.g. to check move ownership), it queries
    GameSession::getSnapshot() -> IGameController::getSnapshot() directly,
    reading straight from Core rather than a second, potentially-stale
    copy. IGameController is otherwise push-only (via the printer
    callback); getSnapshot() is the one on-demand read it offers.

Not yet included: error responses for illegal/rejected moves (both
Core's own illegal-move handling and Server's ownership check currently
just silently no-op), login, matchmaking, or room messages. Those are
separate, larger pieces of work building on top of this.
