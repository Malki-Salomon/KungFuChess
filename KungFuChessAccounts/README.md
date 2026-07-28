Accounts/rating domain logic. Static library, no Boost, no Core dependency.
Deliberately separate from KungFuChessServer (an Application/.exe project,
not linkable) so this logic can be unit-tested directly from
KungFuChessTest via a normal ProjectReference, same as KungFuChessCore.

Pure STL except for two deliberately walled-off exceptions, each confined
to exactly one file:
  - SqliteUserRepository.cpp is the ONLY file in the whole codebase that
    includes sqlite3.h or calls any sqlite3_* function - not even
    SqliteUserRepository.h leaks it (the sqlite3 type is forward-declared
    there).
  - PasswordHasher.cpp is the ONLY file in the whole codebase that
    includes sodium.h or calls any libsodium function - not even
    PasswordHasher.h leaks it.
Everything else in this project, and everything outside it, stays free of
both.

Stages so far:
  - 4a ("Auth foundation, no real crypto/DB yet"): built AuthService,
    EloCalculator, IUserRepository, InMemoryUserRepository, and a
    placeholder (NOT cryptographically secure) PasswordHasher.
  - 4b ("Real SQLite persistence"): added SqliteUserRepository, a second
    IUserRepository implementation, file-backed and durable across
    process restarts. InMemoryUserRepository was NOT removed - it stays,
    still used by the fast AuthService unit tests that don't need a real
    database (AuthServiceTests.cpp); SqliteUserRepository has its own
    parallel test file (AuthServiceSqliteTests.cpp) that re-runs the same
    cases plus a durability test. Server.cpp constructs AuthService with a
    SqliteUserRepository instead of an InMemoryUserRepository - that's the
    only change AuthService's own code needed, since it only ever
    depended on the IUserRepository interface.
  - 4c ("Real password hashing", this stage): replaced PasswordHasher's
    placeholder body with real libsodium (Argon2id via
    crypto_pwhash_str()/crypto_pwhash_str_verify()) - same two-function
    public interface as stage 4a, so AuthService (and everything above it)
    needed zero changes. The placeholder implementation was deleted
    entirely, not left behind disabled. Added PasswordHasher::init(), a
    thin wrapper around libsodium's required one-time sodium_init() call -
    both KungFuChessServer.cpp's and KungFuChessTest.cpp's main() call it
    before anything else runs, so real hashing is never attempted before
    libsodium is ready. Any accounts hashed with the stage 4a/4b
    placeholder will NOT verify against this implementation - expected,
    not a bug (old placeholder hashes and real Argon2id hashes are not
    compatible; delete stale test .db files rather than trying to migrate
    them).

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
    IUserRepository contract. Requires the sqlite3 vcpkg package.
  - PasswordHasher: init()/hash()/verify(). Real, final implementation as
    of this stage - see its header.
  - EloCalculator: standard ELO update, K-factor 32. No staging needed -
    this is the real, final version already.
  - AuthService: registerAccount()/login() against an IUserRepository,
    using PasswordHasher. This is what Server.cpp actually calls; it
    knows nothing about PlayerDirectory (display name) or PlayerAssignment
    (seat/color) - both are populated/consulted separately by the caller.

Vcpkg dependencies (sqlite3, libsodium): both wired the same way - this
project's .vcxproj gets VcpkgRoot + the vcpkg include path (for compiling
SqliteUserRepository.cpp/PasswordHasher.cpp), while whichever .vcxproj
actually links an .exe (KungFuChessServer, KungFuChessTest) gets the
matching AdditionalDependencies (sqlite3.lib, libsodium.lib) - a static
library itself has no link step, so it can't resolve those symbols on its
own; that only happens once something actually links KungFuChessAccounts
into an executable.

Known limitation, NOT fixed yet, and now the most pressing thing left in
this feature: Server.cpp calls AuthService directly from the network I/O
thread (same place PlayerDirectory handling already runs). Every
register/login now does both real (deliberately slow) Argon2id hashing
AND real synchronous disk I/O right there on the I/O thread. Addressed in
stage 4d (moving this off the I/O thread), not here.
