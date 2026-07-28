#pragma once

#include <utility>

// Standard ELO rating update, K-factor = 32. No external dependencies, so
// (unlike PasswordHasher/InMemoryUserRepository) this is the real, final
// version - nothing here needs to change in a later stage.
namespace EloCalculator
{
    enum class Result { WinA, WinB, Draw };

    // Returns {newRatingA, newRatingB}.
    std::pair<int, int> computeNewRatings(int ratingA, int ratingB, Result result);
}
