# KungFuChessAccounts - user accounts, password hashing, Elo and match
# results, behind the IUserRepository interface (SQLite and in-memory
# implementations both live here).
#
# SQLite and libsodium are linked PRIVATE because neither leaks into a public
# header: SqliteUserRepository.h forward-declares `struct sqlite3` and keeps
# sqlite3.h inside the .cpp, and PasswordHasher.h never mentions sodium. So
# consumers get the link dependency (static libs propagate it) without
# inheriting the include paths. The .vcxproj had to name sqlite3.lib and
# libsodium.lib again in every consumer; here it is stated once.

find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)

add_library(KungFuChessAccounts STATIC
    InMemoryUserRepository.cpp
    SqliteUserRepository.cpp
    PasswordHasher.cpp
    EloCalculator.cpp
    AuthService.cpp
    MatchResultService.cpp

    IUserRepository.h
    InMemoryUserRepository.h
    SqliteUserRepository.h
    PasswordHasher.h
    EloCalculator.h
    AuthService.h
    MatchResultService.h
)

target_include_directories(KungFuChessAccounts PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(KungFuChessAccounts
    PRIVATE
        unofficial::sqlite3::sqlite3
        unofficial-sodium::sodium
)

kfc_apply_common_settings(KungFuChessAccounts)
