# KungFuChessSessionState - who is seated where: player assignment, the
# player directory, the room registry and the pending-disconnect tracker.
#
# This library reads SessionId.h (and CommandInbox.h) out of the server's
# Network folder, exactly as KungFuChessSessionState.vcxproj does via
# AdditionalIncludeDirectories=$(ProjectDir)..\KungFuChessServer\Network.
# The include path is reproduced rather than "fixed" by relocating those
# headers - moving them would change the project boundaries this migration
# is required to preserve.

add_library(KungFuChessSessionState STATIC
    PlayerAssignment.cpp
    PlayerDirectory.cpp
    RoomRegistry.cpp
    PendingDisconnectTracker.cpp

    PlayerAssignment.h
    PlayerDirectory.h
    RoomRegistry.h
    PendingDisconnectTracker.h
)

target_include_directories(KungFuChessSessionState
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/KungFuChessServer/Network
)

kfc_apply_common_settings(KungFuChessSessionState)
