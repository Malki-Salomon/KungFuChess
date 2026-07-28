Shared wire-protocol library. Static library, no Boost, no Core
dependency - pure STL. Both KungFuChessServer and KungFuChessShellClient
link against this, so the message formats/parsing logic exist in exactly
one place rather than being duplicated across client and server (matching
the project's existing principle of "one shared source of truth" already
used for CoreFactory).

Every message type here defines BOTH directions (build + parse) in the
same file, deliberately - so a client's build() and the server's parse()
(or vice versa) can't silently drift apart the way independently
hand-written build/parse code in two different files could.

Contents:
  - MiniJson: narrow, purpose-built JSON field extraction (see its header
    for why it's not a general parser). Handles flat string, bool, and
    int fields.
  - LoginMessage: build/parse
    {"type":"login","username":"...","password":"..."}. Sent by a client
    that already has an account.
  - RegisterMessage: build/parse
    {"type":"register","username":"...","password":"..."}. Same shape as
    LoginMessage, different "type" - sent by a client creating a new
    account. Kept as a separate message/file: register and login are
    separate, explicit actions, not one message with an implicit
    auto-create.
  - AuthResultMessage: build/parse
    {"type":"authResult","success":true,"message":"","rating":1200}. The
    server's one reply shape for both a "register" and a "login" attempt.
  - MoveMessage: build/parse {"type":"move","from":"e2","to":"e4"}. Used
    by the client to build a move request, and by MoveTranslator
    (server-side, in KungFuChessServer/Protocol/) to parse it - this file
    owns the message *shape*; MoveTranslator owns turning an already-
    parsed move into Core commands.
  - BoardMessage: build/parse {"type":"board","cells":[...]}. Used by
    SnapshotBroadcaster (server-side) to serialize, and by a client to
    parse for display. Works in plain strings only, not PieceType/
    PieceColor - converting Core's GameSnapshot into that string grid is
    SnapshotBroadcaster's job, not this one's.

NOT here (moved back to KungFuChessServer/Protocol/ - they're genuinely
server-only, the shell client never uses either):
  - MoveNotation: algebraic square <-> board row/col, and Core's
    board-coordinate "click X Y" command - Core-facing, not shared.
  - MoveTranslator: turns an already-parsed move into Core click commands.
    Depends on MoveMessage (here) for parsing the message shape, and
    MoveNotation (also server-side) for the Core-facing translation.
  - SnapshotBroadcaster: requires Core's GameSnapshot/PieceRegistry, which
    this library deliberately has zero dependency on.
