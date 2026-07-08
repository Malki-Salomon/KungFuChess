#include "doctest.h"
#include "Game.h"

TEST_SUITE("Game")
{
    TEST_CASE("Valid board")
    {
        Game game;

        game.setupBoard({
            "wK . .",
            ". bK .",
            ". . ."
            });

        CHECK(game.isBoardValid());
    }

    TEST_CASE("Invalid board")
    {
        Game game;

        game.setupBoard({
            "wK XX ."
            });

        CHECK_FALSE(game.isBoardValid());
    }

    TEST_CASE("Legal king move")
    {
        Game game;

        game.setupBoard({
            "wK . .",
            ". . .",
            ". . ."
            });

        game.executeClick(50, 50);
        game.executeClick(150, 50);

        CHECK(game.isBoardValid());
    }

    TEST_CASE("Illegal king move")
    {
        Game game;

        game.setupBoard({
            "wK . .",
            ". . .",
            ". . ."
            });

        game.executeClick(50, 50);
        game.executeClick(250, 250);

        CHECK(game.isBoardValid());
    }

    TEST_CASE("Wait")
    {
        Game game;

        game.executeWait(1000);

        CHECK(game.isBoardValid());
    }

    TEST_CASE("Capture enemy piece")
    {
        Game game;

        game.setupBoard({
            "wR bP .",
            ". . .",
            ". . ."
            });


        game.executeClick(50, 50);
        game.executeClick(150, 50);


        CHECK(game.isBoardValid());
    }

    TEST_CASE("Cannot capture own piece")
    {
        Game game;

        game.setupBoard({
            "wR wP .",
            ". . .",
            ". . ."
            });


        game.executeClick(50, 50);
        game.executeClick(150, 50);


        CHECK(game.isBoardValid());
    }
}