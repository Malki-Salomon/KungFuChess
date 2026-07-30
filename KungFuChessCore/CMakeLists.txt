# KungFuChessCore - the game itself: board, pieces, rules, the real-time
# arbiter and the command/event plumbing around them. Knows nothing about
# networking, accounts or rendering, and links no external library.
#
# Two deliberate differences from KungFuChessCore.vcxproj:
#
#  * The precompiled header is not reproduced. pch.h contains only
#    `#include "framework.h"`, which is only `#define WIN32_LEAN_AND_MEAN` -
#    a PCH buys nothing there. pch.h remains an ordinary header on the
#    include path, so every existing `#include "pch.h"` still resolves and
#    no source file needed editing.
#
#  * C++17, applied via kfc_apply_common_settings. This project alone had no
#    LanguageStandard set and so built as C++14, even though the server, both
#    clients and the test suite all compile its headers as C++17.
#
# PieceStatus.cpp and RegisterCommands.cpp are tracked in git but are absent
# from KungFuChessCore.vcxproj, so MSBuild does not compile them. They are
# left out here too, so both build systems compile the same translation
# units. (Both are effectively empty today - PieceStatus.cpp is two includes,
# RegisterCommands.cpp is a commented-out function. If they are ever revived,
# add them to the .vcxproj and to the list below together.)

add_library(KungFuChessCore STATIC
    App.cpp
    Bishop.cpp
    Board.cpp
    ClickCommand.cpp
    Command.cpp
    CommandFactory.cpp
    Controller.cpp
    CoreFactory.cpp
    Game.cpp
    GameSnapshot.cpp
    GameStatus.cpp
    IBoardConvert.cpp
    IBoardPrinter.cpp
    ICommandConvert.cpp
    King.cpp
    Knight.cpp
    KungFuChessCore.cpp
    Pawn.cpp
    pch.cpp
    Piece.cpp
    PieceFactory.cpp
    PieceRegistry.cpp
    PieceTypes.cpp
    Position.cpp
    PrintBoardCommand.cpp
    Queen.cpp
    RealTimeArbiter.cpp
    Rook.cpp
    RuleEngine.cpp
    Snapshot.cpp
    SnapshotBoardConvert.cpp
    StringCommandConvert.cpp
    TextBoardConvert.cpp
    TextualBoardPrinting.cpp
    WaitCommand.cpp
    Events/EventBus.cpp
    Events/GameStateChangedEvent.cpp

    App.h
    Bishop.h
    Board.h
    ClickCommand.h
    Command.h
    CommandFactory.h
    Controller.h
    framework.h
    Game.h
    GameSnapshot.h
    GameStatus.h
    IBoardConvert.h
    IBoardPrinter.h
    ICommandConvert.h
    IGameController.h
    King.h
    Knight.h
    Pawn.h
    pch.h
    Piece.h
    PieceFactory.h
    PieceRegistry.h
    PieceTypes.h
    Position.h
    PrintBoardCommand.h
    Queen.h
    RealTimeArbiter.h
    Rook.h
    RuleEngine.h
    Snapshot.h
    SnapshotBoardConvert.h
    StringCommandConvert.h
    TextBoardConvert.h
    TextualBoardPrinting.h
    WaitCommand.h
    Events/Event.h
    Events/EventBus.h
    Events/GameStateChangedEvent.h
)

# Consumers include both "Board.h" and "Events/EventBus.h", so the project
# root is the single include root - matching $(ProjectDir) in the .vcxproj.
target_include_directories(KungFuChessCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

kfc_apply_common_settings(KungFuChessCore)
