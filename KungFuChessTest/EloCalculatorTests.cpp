#include "doctest.h"
#include "EloCalculator.h"

TEST_SUITE("EloCalculator")
{
    TEST_CASE("Upset win gains more rating than the same win would for the favorite")
    {
        std::pair<int, int> upset = EloCalculator::computeNewRatings(1200, 1600, EloCalculator::Result::WinA);
        int underdogGainOnUpset = upset.first - 1200;

        std::pair<int, int> expectedWin = EloCalculator::computeNewRatings(1600, 1200, EloCalculator::Result::WinA);
        int favoriteGainOnExpectedWin = expectedWin.first - 1600;

        CHECK(underdogGainOnUpset > 0);
        CHECK(favoriteGainOnExpectedWin > 0);
        CHECK(underdogGainOnUpset > favoriteGainOnExpectedWin);
    }

    TEST_CASE("Equal ratings: a win gains exactly K/2 and the loss mirrors it")
    {
        std::pair<int, int> result = EloCalculator::computeNewRatings(1400, 1400, EloCalculator::Result::WinA);

        CHECK(result.first - 1400 == 16);
        CHECK(result.second - 1400 == -16);
    }

    TEST_CASE("Equal ratings: a draw leaves both unchanged")
    {
        std::pair<int, int> result = EloCalculator::computeNewRatings(1400, 1400, EloCalculator::Result::Draw);

        CHECK(result.first == 1400);
        CHECK(result.second == 1400);
    }

    TEST_CASE("WinB credits B and debits A")
    {
        std::pair<int, int> result = EloCalculator::computeNewRatings(1200, 1200, EloCalculator::Result::WinB);

        CHECK(result.first < 1200);
        CHECK(result.second > 1200);
    }
}
