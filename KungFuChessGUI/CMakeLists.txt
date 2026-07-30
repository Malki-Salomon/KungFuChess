# KungFuChessGUI - the OpenCV-based graphical client. Only built when
# KFC_BUILD_GUI is ON (the root CMakeLists defaults it to whether OpenCV was
# found), so headless and container builds skip it without failing.

find_package(OpenCV CONFIG REQUIRED)

add_executable(KungFuChessGUI
    GameLoop.cpp
    img.cpp
    InputHandler.cpp
    KungFuChessGUI.cpp
    GameWindow.cpp
    NetworkSnapshotAdapter.cpp
    TextureManager.cpp
    Layout.cpp
    BoardRenderer.cpp
    BoardAnimationController.cpp
    SpriteAnimationClipLoader.cpp
    SpriteAnimationCatalog.cpp
    SpriteAnimationStateMachine.cpp
    PieceAssetPaths.cpp

    BoardAnimationController.h
    BoardRenderer.h
    GameLoop.h
    GameWindow.h
    img.hpp
    IMoveIntentHint.h
    InputHandler.h
    Layout.h
    NetworkSnapshotAdapter.h
    PieceAssetPaths.h
    SpriteAnimationCatalog.h
    SpriteAnimationClipLoader.h
    SpriteAnimationStateMachine.h
    SpriteAnimationTypes.h
    TextureManager.h
)

target_include_directories(KungFuChessGUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(KungFuChessGUI
    PRIVATE
        KungFuChessCore
        KungFuChessClientCommon
        KungFuChessProtocol
        KungFuChessLogging
        ${OpenCV_LIBS}
)

kfc_link_platform_sockets(KungFuChessGUI)
kfc_apply_common_settings(KungFuChessGUI)

# The GUI loads its assets through paths relative to the working directory
# (GameWindow.cpp: R"(.\pictures\board_classic.png)"), which under MSBuild
# resolves because Visual Studio launches the exe from $(ProjectDir). A CMake
# binary directory has no pictures/ folder, so mirror it next to the exe.
add_custom_command(TARGET KungFuChessGUI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/pictures"
            "$<TARGET_FILE_DIR:KungFuChessGUI>/pictures"
    COMMENT "Copying sprite assets next to KungFuChessGUI"
    VERBATIM
)
