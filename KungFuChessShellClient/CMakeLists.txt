# KungFuChessShellClient - the console client. Connects to localhost:9000,
# runs the login flow and prints the board as text. No OpenCV, no Boost of
# its own: all networking is borrowed from KungFuChessClientCommon.

add_executable(KungFuChessShellClient
    main.cpp
)

target_link_libraries(KungFuChessShellClient
    PRIVATE
        KungFuChessClientCommon
        KungFuChessProtocol
        KungFuChessLogging
)

kfc_link_platform_sockets(KungFuChessShellClient)
kfc_apply_common_settings(KungFuChessShellClient)
