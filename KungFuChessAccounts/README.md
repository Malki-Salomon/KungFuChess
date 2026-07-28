Accounts/rating domain logic. Static library, no Boost, no Core dependency.
Deliberately separate from KungFuChessServer (an Application/.exe project,
not linkable) so this logic can be unit-tested directly from
KungFuChessTest via a normal ProjectReference, same as KungFuChessCore.

Pure STL except for one deliberately walled-off exception:
SqliteUserRepository.cpp is the ONLY file in this entire project (and the
only file in the whole codebase) that includes sqlite3.h or calls any
sqlite3_* function - not even SqliteUserRepository.h leaks it (the sqlite3
type is forward-declared there). Everything else in this project, and
everything outside it, stays sqlite-free.

Stages so far:
  - 4a ("Auth foundation, no real crypto/DB yet"): built AuthService,
    EloCalculator, IUserRepository, InMemoryUserRepository, and a
    placeholder PasswordHasher.
  - 4b ("Real SQLite persistence", this stage): added SqliteUserRepository,
    a second IUserRepository implementation, file-backed and durable
    across process restarts. InMemoryUserRepository was NOT removed - it
    stays, still used by the fast AuthService unit tests that don't need a
    real database (AuthServiceTests.cpp); SqliteUserRepository has its own
    parallel test file (AuthServiceSqliteTests.cpp) that re-runs the same
    cases plus a durability test. Server.cpp now constructs AuthService
    with a SqliteUserRepository instead of an InMemoryUserRepository -
    that's the only change AuthService's own code needed, since it only
    ever depended on the IUserRepository interface.

One piece is still a temporary placeholder, explicitly staged to be
swapped out later without anything above it changing:
  - PasswordHasher: NOT cryptographically secure yet - see its header.
    Replaced with real libsodium/Argon2id in a later stage.

Contents:
  - IUserRepository: storage interface (username exists / create account /
    get password hash / get+update rating). AuthService depends on this
    interface, never on a concrete implementation.
  - InMemoryUserRepository: in-process only, nothing survives a restart.
    Kept around for fast unit tests that don't need a real database.
  - SqliteUserRepository: real, file-backed persistence. Constructor takes
    the .db file path (not hardcoded - Server.cpp decides where it lives,
    currently a fixed relative path, "kungfuchess.db", next to the
    executable - no config system exists yet, see
    KungFuChessServer/Infrastructure/README.md). Table/column layout is
    this class's own private implementation detail, not part of the
    IUserRepository contract. Requires the sqlite3 vcpkg package
    (`vcpkg install sqlite3:x64-windows`) - wired into this project's
    .vcxproj (VcpkgRoot property, include path) for compiling, and into
    whichever .vcxproj actually links an .exe (KungFuChessServer,
    KungFuChessTest) for resolving sqlite3.lib at link time, since a
    static library itself has no link step.
  - PasswordHasher: hash/verify. Real, final interface; placeholder body.
  - EloCalculator: standard ELO update, K-factor 32. No staging needed -
    this is the real, final version already.
  - AuthService: registerAccount()/login() against an IUserRepository,
    using PasswordHasher. This is what Server.cpp actually calls; it
    knows nothing about PlayerDirectory (display name) or PlayerAssignment
    (seat/color) - both are populated/consulted separately by the caller.

Known limitation, NOT fixed yet: Server.cpp calls AuthService directly
from the network I/O thread (same place PlayerDirectory handling already
runs). As of this stage that's no longer harmless - every register/login
now does real, synchronous disk I/O right there on the I/O thread. Gets
worse once a later stage adds real, deliberately-slow Argon2id hashing on
top. Addressed in a later stage (moving this off the I/O thread), not
here.
