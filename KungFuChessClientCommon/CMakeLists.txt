# KungFuChessClientCommon - the parts both clients share: the Boost.Beast
# WebSocket client, the console login flow and the move sender.
#
# Boost stays PRIVATE. WebSocketClient.h exposes only <functional>, <memory>
# and <string> - the Beast and Asio types are confined to WebSocketClient.cpp
# - so neither the shell client nor the GUI needs Boost on its include path.

find_package(Boost CONFIG REQUIRED COMPONENTS asio beast)

add_library(KungFuChessClientCommon STATIC
    WebSocketClient.cpp
    ConsoleLoginFlow.cpp
    MoveSender.cpp

    WebSocketClient.h
    ConsoleLoginFlow.h
    MoveSender.h
)

target_include_directories(KungFuChessClientCommon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(KungFuChessClientCommon
    PUBLIC
        KungFuChessProtocol
        KungFuChessLogging
    PRIVATE
        Boost::asio
        Boost::beast
)

kfc_link_platform_sockets(KungFuChessClientCommon)
kfc_apply_common_settings(KungFuChessClientCommon)
