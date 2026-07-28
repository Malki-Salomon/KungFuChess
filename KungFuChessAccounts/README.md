Accounts/rating domain logic. Static library, no Boost, no Core dependency,
no external dependencies at all - pure STL. Deliberately separate from
KungFuChessServer (an Application/.exe project, not linkable) so this
logic can be unit-tested directly from KungFuChessTest via a normal
ProjectReference, same as KungFuChessCore.

Stage 4a ("Auth foundation, no real crypto/DB yet"). Two of these pieces
are temporary placeholders, explicitly staged to be swapped out later
without anything above them changing:
  - PasswordHasher: NOT cryptographically secure yet - see its header.
    Replaced with real libsodium/Argon2id in a later stage.
  - InMemoryUserRepository: in-process only, nothing survives a restart.
    Replaced (or joined) by a SqliteUserRepository in a later stage.

Contents:
  - IUserRepository: storage interface (username exists / create account /
    get password hash / get+update rating). AuthService depends on this
    interface, never on a concrete implementation.
  - InMemoryUserRepository: the only IUserRepository implementation this
    stage builds.
  - PasswordHasher: hash/verify. Real, final interface; placeholder body.
  - EloCalculator: standard ELO update, K-factor 32. No staging needed -
    this is the real, final version already.
  - AuthService: registerAccount()/login() against an IUserRepository,
    using PasswordHasher. This is what Server.cpp actually calls; it
    knows nothing about PlayerDirectory (display name) or PlayerAssignment
    (seat/color) - both are populated/consulted separately by the caller.

Known limitation, not fixed in this stage: Server.cpp currently calls
AuthService directly from the network I/O thread (same place
PlayerDirectory handling already runs). Fine while PasswordHasher is
instant and InMemoryUserRepository is in-memory; stops being fine once a
later stage adds real (deliberately slow) Argon2id hashing and real disk
I/O. Addressed in a later stage, not here.
