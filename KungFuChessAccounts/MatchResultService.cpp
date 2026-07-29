#include "MatchResultService.h"

#include "EloCalculator.h"

namespace
{
    EloCalculator::Result toEloResult(MatchOutcome outcome)
    {
        switch (outcome)
        {
            case MatchOutcome::WhiteWon: return EloCalculator::Result::WinA;
            case MatchOutcome::BlackWon: return EloCalculator::Result::WinB;
            case MatchOutcome::Draw:     return EloCalculator::Result::Draw;
        }
        return EloCalculator::Result::Draw;
    }
}

MatchResultService::MatchResultService(IUserRepository& repo)
    : m_repo(repo)
{
}

MatchResult MatchResultService::recordResult(const std::string& whiteUsername,
                                              const std::string& blackUsername,
                                              MatchOutcome outcome)
{
    int whiteRating = m_repo.getRating(whiteUsername);
    int blackRating = m_repo.getRating(blackUsername);

    auto [newWhiteRating, newBlackRating] = EloCalculator::computeNewRatings(whiteRating, blackRating, toEloResult(outcome));

    m_repo.updateRating(whiteUsername, newWhiteRating);
    m_repo.updateRating(blackUsername, newBlackRating);

    return { newWhiteRating, newBlackRating };
}
