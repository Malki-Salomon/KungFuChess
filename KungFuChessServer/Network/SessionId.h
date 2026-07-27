#pragma once

#include <cstdint>

// Identifies one connected client for as long as it stays connected.
// Assigned by WebSocketServer when a connection completes its handshake.
using SessionId = std::uint64_t;

constexpr SessionId kInvalidSessionId = 0;
