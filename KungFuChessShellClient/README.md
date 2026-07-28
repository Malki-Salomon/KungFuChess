Shell-based login stage: a standalone console client, not the GUI.

Flow: prompt for a username (no password, no validation, no persistence -
purely for display/identification at this stage) -> connect to the
server over WebSocket -> send a login message -> loop printing board
updates (received on a background thread) while accepting move input
from the terminal ("e2 e4", or "quit" to exit).

Files:
  - WebSocketClient.h/.cpp: blocking Boost.Beast WS client. connect()/
    send() block the calling thread; startReceiveLoop() spawns one
    background thread that blocks on reads. NOT verified by compiling
    against real Boost in the environment these files were authored in
    (no Boost headers available there) - treat as a solid first draft,
    same caveat as WebSocketServer.cpp had originally.
  - main.cpp: the login/move loop described above.

Links against KungFuChessProtocol (LoginMessage, MoveMessage,
BoardMessage) - no protocol logic is duplicated here.

Not included yet: password/persistence (next stage), any GUI, spectator
mode, reconnect handling, or acting on the "assigned color" message the
server sends after login (currently just ignored - this client only
understands board updates).
