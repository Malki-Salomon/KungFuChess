#include "doctest.h"
#include "MatchResultService.h"
#include "InMemoryUserRepository.h"

namespace
{
    void registerWithRating(InMemoryUserRepository& repo, const std::string& username, int rating)
    {
        repo.createAccount(username, "irrelevant-hash");
        repo.updateRating(username, rating);
    }
}

TEST_SUITE("MatchResultService")
{
    TEST_CASE("White wins: white gains rating, black loses it, and the repository reflects it")
    {
        InMemoryUserRepository repo;
        registerWithRating(repo, "alice", 1200);
        registerWithRating(repo, "bob", 1200);
        MatchResultService service(repo);

        MatchResult result = service.recordResult("alice", "bob", MatchOutcome::WhiteWon);

        CHECK(result.whiteNewRating > 1200);
        CHECK(result.blackNewRating < 1200);
        CHECK(repo.getRating("alice") == result.whiteNewRating);
        CHECK(repo.getRating("bob") == result.blackNewRating);
    }

    TEST_CASE("Black wins: black gains rating, white loses it, and the repository reflects it")
    {
        InMemoryUserRepository repo;
        registerWithRating(repo, "alice", 1200);
        registerWithRating(repo, "bob", 1200);
        MatchResultService service(repo);

        MatchResult result = service.recordResult("alice", "bob", MatchOutcome::BlackWon);

        CHECK(result.whiteNewRating < 1200);
        CHECK(result.blackNewRating > 1200);
        CHECK(repo.getRating("alice") == result.whiteNewRating);
        CHECK(repo.getRating("bob") == result.blackNewRating);
    }

    TEST_CASE("Draw between equal ratings leaves both unchanged, and the repository reflects it")
    {
        InMemoryUserRepository repo;
        registerWithRating(repo, "alice", 1200);
        registerWithRating(repo, "bob", 1200);
        MatchResultService service(repo);

        MatchResult result = service.recordResult("alice", "bob", MatchOutcome::Draw);

        CHECK(result.whiteNewRating == 1200);
        CHECK(result.blackNewRating == 1200);
        CHECK(repo.getRating("alice") == 1200);
        CHECK(repo.getRating("bob") == 1200);
    }

    TEST_CASE("An upset win gains the underdog more than the same win would for the favorite")
    {
        InMemoryUserRepository upsetRepo;
        registerWithRating(upsetRepo, "underdog", 1200);
        registerWithRating(upsetRepo, "favorite", 1600);
        MatchResultService upsetService(upsetRepo);
        MatchResult upset = upsetService.recordResult("underdog", "favorite", MatchOutcome::WhiteWon);
        int underdogGain = upset.whiteNewRating - 1200;

        InMemoryUserRepository expectedRepo;
        registerWithRating(expectedRepo, "favorite", 1600);
        registerWithRating(expectedRepo, "underdog", 1200);
        MatchResultService expectedService(expectedRepo);
        MatchResult expectedWin = expectedService.recordResult("favorite", "underdog", MatchOutcome::WhiteWon);
        int favoriteGain = expectedWin.whiteNewRating - 1600;

        CHECK(underdogGain > 0);
        CHECK(favoriteGain > 0);
        CHECK(underdogGain > favoriteGain);
    }
}
