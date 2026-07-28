Server-only protocol pieces - things that need Core (GameSnapshot,
PieceRegistry) and/or translate into Core's own command format, which is
exactly why these live here rather than in the shared KungFuChessProtocol
library (which has zero Core dependency, by design - see its README).

Contents:
  - MoveNotation: algebraic square ("e2") <-> board row/col, and the
    board-coordinate "click X Y" command Core's StringCommandConvert
    actually understands. Core-facing; nothing here understands JSON.
  - MoveTranslator: parseMove() delegates to the shared MoveMessage::parse()
    for the JSON shape (so it can't drift from what a client's build()
    produces), then uses MoveNotation to turn the parsed squares into
    board coordinates. toClickCommands() then builds the actual Core
    commands. Split into parse + build so Server can check move ownership
    against the parsed squares before committing to dispatching anything.
  - SnapshotBroadcaster: converts a Core GameSnapshot into a plain grid of
    symbol strings (PieceRegistry's whiteSymbol/blackSymbol, e.g. "wP"),
    then hands that grid to the shared BoardMessage::build() to actually
    serialize it - so the JSON shape itself is defined in exactly one
    place (BoardMessage), shared with whatever parses it client-side.
    Broadcasting is this class's only job - it does not cache board
    state. When Server needs to know what the board currently looks like
    (e.g. to check move ownership), it queries GameSession::getSnapshot()
    -> IGameController::getSnapshot() directly, reading straight from Core
    rather than a second, potentially-stale copy here.

Not yet included: error responses for illegal/rejected moves (both
Core's own illegal-move handling and Server's ownership check currently
just silently no-op), matchmaking, or room messages. Those are separate,
larger pieces of work building on top of this.
