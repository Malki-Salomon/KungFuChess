# KungFuChessLogging - shared persistent logging, used by both the server
# and the clients. No external dependencies.

add_library(KungFuChessLogging STATIC
    Logger.cpp
    Logger.h
)

target_include_directories(KungFuChessLogging PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

kfc_apply_common_settings(KungFuChessLogging)
