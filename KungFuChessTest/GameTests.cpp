#include "doctest.h"
#include "Game.h"

TEST_SUITE("Game Simulations")
{
    TEST_CASE("Standard simulation match format")
    {
        Game game;

        game.runSimulation({
            "Board:",
            "wR . .",
            ". . .",
            "bR . .",
            "Commands:",
            "click 50 50",
            "click 250 50",
            "click 50 250",
            "click 250 250",
            "wait 2000",
            "print board"
            });

        CHECK(game.getBoard().isEmpty(Position(0, 0)));
        CHECK(game.getBoard().getPieceType(Position(0, 2)) == PieceType::Rook);
    }

    TEST_CASE("Another simulation test case")
    {
        Game game;

        game.runSimulation({
            "Board:",
            "wK . .",
            ". . .",
            "Commands:",
            "click 50 50",
            "click 150 50",
            "wait 1000"
            });

        CHECK(game.getBoard().getPieceType(Position(0, 1)) == PieceType::King);
    }
}