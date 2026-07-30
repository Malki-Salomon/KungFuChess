# KungFuChessTest - the doctest suite. doctest is vendored (doctest.h in this
# folder), so there is no package to find.
#
# KungFuChessTest.cpp defines its own main() rather than using
# DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN, because libsodium needs one process-wide
# PasswordHasher::init() before any test touches real hashing.

set(KFC_TEST_SOURCES
    AppTest.cpp
    BishopTests.cpp
    BoardTest.cpp
    CommandFactoryTest.cpp
    EventBusTests.cpp
    GameTests.cpp
    KingTests.cpp
    KnightTests.cpp
    KungFuChessTest.cpp
    PieceTests.cpp
    PrinterAdapterTests.cpp
    QueenTests.cpp
    RookTests.cpp
    RuleEngineTest.cpp
    TextBoardConvertTest.cpp
    EloCalculatorTests.cpp
    PasswordHasherTests.cpp
    AuthServiceTests.cpp
    AuthServiceSqliteTests.cpp
    MatchResultServiceTests.cpp
    PlayerAssignmentTests.cpp
    PlayerDirectoryTests.cpp
    AssignedMessageTests.cpp
    RoomRegistryTests.cpp
    RoomMessagesTests.cpp
    PendingDisconnectTrackerTests.cpp
    DisconnectMessagesTests.cpp
    LoggerTests.cpp
    PieceRegistryTests.cpp
    JumpMessageTests.cpp
    NetworkSnapshotAdapterTests.cpp
    SquareNotationTests.cpp

    # NetworkSnapshotAdapter belongs to KungFuChessGUI, which is an executable
    # and therefore has no objects to link against. KungFuChessTest.vcxproj
    # deals with that by compiling this one file directly into the test
    # binary; the same trick is used here, unchanged.
    #
    # Note this is intentionally NOT guarded by KFC_BUILD_GUI. The file pulls
    # in Core, Protocol and Logging but no OpenCV, so the suite stays complete
    # in a headless build.
    ../KungFuChessGUI/NetworkSnapshotAdapter.cpp
)

# Version-info resource; MSVC-only, as in the .vcxproj.
if(MSVC)
    list(APPEND KFC_TEST_SOURCES KungFuChessTest.rc)
endif()

add_executable(KungFuChessTest ${KFC_TEST_SOURCES})

# Everything except the GUI folder arrives through the linked targets' PUBLIC
# include directories (Server/Network, for instance, rides along on
# KungFuChessSessionState). The GUI is not a link dependency, so its header
# path has to be named explicitly.
target_include_directories(KungFuChessTest
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/KungFuChessGUI
)

target_link_libraries(KungFuChessTest
    PRIVATE
        KungFuChessCore
        KungFuChessProtocol
        KungFuChessAccounts
        KungFuChessSessionState
        KungFuChessLogging
        KungFuChessClientCommon
)

kfc_apply_common_settings(KungFuChessTest)

# One ctest entry for the whole binary. The suite writes scratch files into
# its working directory (test_*.db from AuthServiceSqliteTests,
# logger_unit_test.log from LoggerTests), so point it at the build tree to
# keep that out of the source tree.
add_test(NAME KungFuChessTest COMMAND KungFuChessTest)

set_tests_properties(KungFuChessTest PROPERTIES
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)
