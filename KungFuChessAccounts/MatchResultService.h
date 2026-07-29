#pragma once

#include "IUserRepository.h"

#include <string>

// The three outcomes a finished match can have, in this library's own
// vocabulary - deliberately not Core's GameStatus (Playing/WhiteWins/
// BlackWins/Draw). This library must not depend on Core, so translating
// GameStatus into a MatchOutcome is the caller's job (KungFuChessServer's
// GameEndCoordinator, which already legitimately depends on Core), same
// reasoning as PlayerAssignment's Seat enum knowing nothing about
// chess/colors.
enum class MatchOutcome { WhiteWon, BlackWon, Draw };

struct MatchResult
{
    int whiteNewRating;
    int blackNewRating;
};

// Runs EloCalculator against both players' current ratings and persists
// the results via IUserRepository. This is the ONE place that turns "a
// match finished with this outcome" into an actual rating update - callers
// don't touch EloCalculator or IUserRepository::updateRating() directly.
class MatchResultService
{
public:
    explicit MatchResultService(IUserRepository& repo);

    // Precondition: both usernames already exist (same precondition
    // IUserRepository::getRating() itself has - see its header).
    MatchResult recordResult(const std::string& whiteUsername,
                              const std::string& blackUsername,
                              MatchOutcome outcome);

private:
    IUserRepository& m_repo;
};
