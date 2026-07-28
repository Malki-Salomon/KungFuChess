Shell-based auth stage: a standalone console client, not the GUI.

Flow: ask whether to register or log in -> prompt for username/password
(entered in plain text - no masking at this stage) -> connect to the
server over WebSocket -> send a register or login message -> block
waiting for the server's authResult reply (WebSocketClient::receiveOne())
and print success/failure. If registration succeeds, the client
immediately follows up with a separate login message using the same
credentials (still two distinct messages - the server never treats a
successful register as an implicit login). On a failed register or
login, the client prints the reason and exits. On a successful login, it
switches to looping: printing board updates (received on a background
thread via startReceiveLoop()) while accepting move input from the
terminal ("e2 e4", or "quit" to exit).

Files:
  - WebSocketClient.h/.cpp: blocking Boost.Beast WS client. connect()/
    send()/receiveOne() block the calling thread; startReceiveLoop()
    spawns one background thread that blocks on reads - receiveOne() and
    startReceiveLoop() must not be used concurrently (see header).
    close() shuts down + closes the underlying socket (not cancel() -
    that only affects Asio's async operation table, not a thread parked
    in a synchronous read) so the background receive thread's blocked
    read actually unblocks and close() doesn't hang; verified against a
    real build and a real server (quit exits cleanly, no kill needed).
  - main.cpp: the register/login/move loop described above.

Links against KungFuChessProtocol (LoginMessage, RegisterMessage,
AuthResultMessage, MoveMessage, BoardMessage) - no protocol logic is
duplicated here.

Not included yet: password persistence across server restarts (server-
side, see KungFuChessAccounts - stage 4a is in-memory only), password
masking on input, real crypto (stage 4a's PasswordHasher is an explicit
placeholder), any GUI, spectator mode, reconnect handling, or acting on
the "assigned color" message the server sends after login (currently
just ignored - this client only understands board updates and authResult).
