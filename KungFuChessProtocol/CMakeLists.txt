# KungFuChessProtocol - wire message types and the minimal JSON codec they
# share. Deliberately dependency-free: it is linked by the server, both
# clients and the test suite, and must stay portable across all of them.

add_library(KungFuChessProtocol STATIC
    MiniJson.cpp
    SquareNotation.cpp
    LoginMessage.cpp
    RegisterMessage.cpp
    AuthResultMessage.cpp
    MoveMessage.cpp
    JumpMessage.cpp
    BoardMessage.cpp
    GameOverMessage.cpp
    AssignedMessage.cpp
    CreateRoomMessage.cpp
    JoinRoomMessage.cpp
    RoomCreatedMessage.cpp
    RoomJoinedMessage.cpp
    RoomJoinFailedMessage.cpp
    OpponentDisconnectedMessage.cpp
    OpponentReconnectedMessage.cpp

    MiniJson.h
    SquareNotation.h
    LoginMessage.h
    RegisterMessage.h
    AuthResultMessage.h
    MoveMessage.h
    JumpMessage.h
    BoardMessage.h
    GameOverMessage.h
    AssignedMessage.h
    CreateRoomMessage.h
    JoinRoomMessage.h
    RoomCreatedMessage.h
    RoomJoinedMessage.h
    RoomJoinFailedMessage.h
    OpponentDisconnectedMessage.h
    OpponentReconnectedMessage.h
)

target_include_directories(KungFuChessProtocol PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

kfc_apply_common_settings(KungFuChessProtocol)
