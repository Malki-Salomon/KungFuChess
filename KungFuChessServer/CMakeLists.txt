# KungFuChessServer - the authoritative game server: WebSocket listener,
# room manager, auth worker and the snapshot broadcaster that drives clients.
#
# Boost, SQLite and libsodium are all PRIVATE. The .vcxproj repeated
# sqlite3.lib and libsodium.lib here even though only KungFuChessAccounts
# calls them; that duplication is dropped because the static-library
# dependency already propagates through the link.

find_package(Boost CONFIG REQUIRED COMPONENTS asio beast)

add_executable(KungFuChessServer
    KungFuChessServer.cpp
    Application/Server.cpp
    Application/GameSession.cpp
    Application/GameFactory.cpp
    Application/SeatColorNaming.cpp
    Application/AuthWorker.cpp
    Application/GameEndCoordinator.cpp
    Application/Room.cpp
    Application/RoomManager.cpp
    Network/WebSocketServer.cpp
    Protocol/SnapshotBroadcaster.cpp
    Protocol/MoveTranslator.cpp
    Protocol/MoveNotation.cpp

    Application/Server.h
    Application/GameSession.h
    Application/GameFactory.h
    Application/SeatColorNaming.h
    Application/AuthWorker.h
    Application/GameEndCoordinator.h
    Application/Room.h
    Application/RoomManager.h
    Network/WebSocketServer.h
    Network/CommandInbox.h
    Network/SessionId.h
    Protocol/SnapshotBroadcaster.h
    Protocol/MoveTranslator.h
    Protocol/MoveNotation.h
)

# Sources here include their siblings both qualified ("Application/Server.h")
# and bare ("Server.h"), so the project root and each subfolder are on the
# path - matching AdditionalIncludeDirectories in KungFuChessServer.vcxproj.
target_include_directories(KungFuChessServer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Application
        ${CMAKE_CURRENT_SOURCE_DIR}/Network
        ${CMAKE_CURRENT_SOURCE_DIR}/Protocol
)

target_link_libraries(KungFuChessServer
    PRIVATE
        KungFuChessCore
        KungFuChessProtocol
        KungFuChessAccounts
        KungFuChessSessionState
        KungFuChessLogging
        Boost::asio
        Boost::beast
)

kfc_link_platform_sockets(KungFuChessServer)
kfc_apply_common_settings(KungFuChessServer)
