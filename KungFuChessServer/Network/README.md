Networking layer.

Current state:
- WebSocketServer (Boost.Beast) accepts connections, reads text messages,
  and can broadcast text to all connected clients.
- Server wires it to the one existing GameSession (POC - single session,
  no per-connection routing yet):
    incoming text  -> CommandInbox -> GameSession::dispatchCommand()
    board changes  -> SnapshotBroadcaster -> WebSocketServer::broadcast()
- Still plain text, not a real protocol: commands are passed through
  verbatim in the same "click x y" / "jump x y" format Core already
  parses, and snapshots are the same text grid TextualBoardPrinting prints
  to the console. Good enough to prove the pipe works end to end; expect
  a real (likely JSON) Protocol layer to replace this once multiple
  sessions/connections need to be told apart.
