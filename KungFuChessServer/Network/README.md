Networking layer - pure transport, knows nothing about the game.

Current state:
- WebSocketServer (Boost.Beast) accepts connections, tracks each one's
  SessionId, reads/receives text messages, and can broadcast or send to
  one specific connection.
- CommandInbox: thread-safe hand-off of (sender SessionId, raw text) from
  the network I/O thread to the game tick thread.

Deliberately has zero dependency on Core, Application, or Protocol - it
only deals in SessionId + std::string, with everything else injected as
std::function callbacks. SnapshotBroadcaster (JSON serialization, board
awareness) lives in Protocol/, not here, even though it calls into
WebSocketServer::broadcast() - see Protocol/README.md.
