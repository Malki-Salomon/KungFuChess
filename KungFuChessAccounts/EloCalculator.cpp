#include "EloCalculator.h"

#include <cmath>

namespace EloCalculator
{
    namespace
    {
        constexpr int kFactor = 32;

        double expectedScore(int ratingSelf, int ratingOpponent)
        {
            return 1.0 / (1.0 + std::pow(10.0, (ratingOpponent - ratingSelf) / 400.0));
        }
    }

    std::pair<int, int> computeNewRatings(int ratingA, int ratingB, Result result)
    {
        double scoreA = 0.5;
        double scoreB = 0.5;
        switch (result)
        {
            case Result::WinA:
                scoreA = 1.0;
                scoreB = 0.0;
                break;
            case Result::WinB:
                scoreA = 0.0;
                scoreB = 1.0;
                break;
            case Result::Draw:
                scoreA = 0.5;
                scoreB = 0.5;
                break;
        }

        double expectedA = expectedScore(ratingA, ratingB);
        double expectedB = expectedScore(ratingB, ratingA);

        int newRatingA = ratingA + static_cast<int>(std::lround(kFactor * (scoreA - expectedA)));
        int newRatingB = ratingB + static_cast<int>(std::lround(kFactor * (scoreB - expectedB)));

        return { newRatingA, newRatingB };
    }
}
