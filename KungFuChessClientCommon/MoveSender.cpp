#include "MoveSender.h"

#include "WebSocketClient.h"
#include "MoveMessage.h"
#include "JumpMessage.h"

MoveSender::MoveSender(WebSocketClient& client)
    : client(client)
{
}

void MoveSender::sendMove(const std::string& from, const std::string& to)
{
    client.send(MoveMessage::build(from, to));
}

void MoveSender::sendJump(const std::string& square)
{
    client.send(JumpMessage::build(square));
}
