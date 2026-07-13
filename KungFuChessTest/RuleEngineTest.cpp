#include "RuleEngineTest.h"
#include "doctest.h"
#include "Board.h"
#include "TextBoardConvert.h"
#include "Position.h"
#include "RuleEngine.h"

TEST_SUITE("RuleEngine")
{
    TEST_CASE("Horizontal path clear")
    {
        Board board;

        TextBoardConvert converter({
            "wR . . ."
            });

        CHECK(converter.Convert(board));

        CHECK(RuleEngine::isPathClear(board, Position(0, 0), Position(0, 3)));
    }

    TEST_CASE("Horizontal path blocked")
    {
        Board board;

        TextBoardConvert converter({
            "wR . bP ."
            });

        CHECK(converter.Convert(board));

        CHECK_FALSE(RuleEngine::isPathClear(board, Position(0, 0), Position(0, 3)));
    }

    TEST_CASE("Vertical path clear")
    {
        Board board;

        TextBoardConvert converter({
            "wR",
            ".",
            ".",
            "."
            });

        CHECK(converter.Convert(board));

        CHECK(RuleEngine::isPathClear(board, Position(0, 0), Position(3, 0)));
    }

    TEST_CASE("Diagonal path clear")
    {
        Board board;

        TextBoardConvert converter({
            "wB . . .",
            ". . . .",
            ". . . .",
            ". . . ."
            });

        CHECK(converter.Convert(board));

        CHECK(RuleEngine::isPathClear(board, Position(0, 0), Position(3, 3)));
    }

    TEST_CASE("Diagonal path blocked")
    {
        Board board;

        TextBoardConvert converter({
            "wB . . .",
            ". wP . .",
            ". . . .",
            ". . . ."
            });

        CHECK(converter.Convert(board));

        CHECK_FALSE(RuleEngine::isPathClear(board, Position(0, 0), Position(3, 3)));
    }

    TEST_CASE("Legal move - Rook")
    {
        Board board;

        TextBoardConvert converter({
            "wR . . ."
            });

        CHECK(converter.Convert(board));

        CHECK(RuleEngine::isLegalMove(board, Position(0, 0), Position(0, 3)));
    }

    TEST_CASE("Illegal move - Rook")
    {
        Board board;

        TextBoardConvert converter({
            "wR . . ."
            });

        CHECK(converter.Convert(board));

        CHECK_FALSE(RuleEngine::isLegalMove(board, Position(0, 0), Position(1, 1)));
    }

    TEST_CASE("Cannot capture own piece")
    {
        Board board;

        TextBoardConvert converter({
            "wR . wP"
            });

        CHECK(converter.Convert(board));

        CHECK_FALSE(RuleEngine::isLegalMove(board, Position(0, 0), Position(0, 2)));
    }

    TEST_CASE("Capture enemy piece")
    {
        Board board;

        TextBoardConvert converter({
            "wR . bP"
            });

        CHECK(converter.Convert(board));

        CHECK(RuleEngine::isLegalMove(board, Position(0, 0), Position(0, 2)));
    }

    TEST_CASE("Move from empty square")
    {
        Board board;

        TextBoardConvert converter({
            ". . ."
            });

        CHECK(converter.Convert(board));

        CHECK_FALSE(RuleEngine::isLegalMove(board, Position(0, 0), Position(0, 2)));
    }

    TEST_CASE("Move outside board")
    {
        Board board;

        TextBoardConvert converter({
            "wR . ."
            });

        CHECK(converter.Convert(board));

        CHECK_FALSE(RuleEngine::isLegalMove(board, Position(0, 0), Position(0, 5)));
    }

    TEST_CASE("Knight can jump over pieces")
    {
        Board board;

        TextBoardConvert converter({
            "wN wP .",
            "wP wP .",
            ".  .  ."
            });

        CHECK(converter.Convert(board));

        CHECK(RuleEngine::isLegalMove(board, Position(0, 0), Position(2, 1)));
    }

    TEST_CASE("Move to the exact same square is illegal")
    {
        Board board;
        

        TextBoardConvert converter({
            "wR . ."
            });

        CHECK(converter.Convert(board));

        CHECK_FALSE(RuleEngine::isLegalMove(board, Position(0, 0), Position(0, 0)));
    }

    TEST_CASE("Path clear for adjacent square (distance of 1)")
    {
        Board board;
        

        TextBoardConvert converter({
            "wR bP ."
            });

        CHECK(converter.Convert(board));

        CHECK(RuleEngine::isPathClear(board, Position(0, 0), Position(0, 1)));
    }

    TEST_CASE("isPathClear - Move of exactly one square (Adjacent)")
    {
        Board board;
        

        TextBoardConvert converter({
            "wR bP . ."
            });
        CHECK(converter.Convert(board));

        CHECK(RuleEngine::isPathClear(board, Position(0, 0), Position(0, 1)));
    }

    TEST_CASE("isLegalMove - Knight ignores blocked path")
    {
        Board board;
        

        TextBoardConvert converter({
            "wN wP .",
            "wP wP .",
            ".  .  ."
            });
        CHECK(converter.Convert(board));

        CHECK(RuleEngine::isLegalMove(board, Position(0, 0), Position(2, 1)));
    }

    TEST_CASE("isLegalMove - King and Pawn do not trigger long path check")
    {
        Board board;
        
        TextBoardConvert converter({
            "wK . bR",
            "wP . bR"
            });
        CHECK(converter.Convert(board));

        CHECK(RuleEngine::isLegalMove(board, Position(0, 0), Position(0, 1)));

        CHECK_FALSE(RuleEngine::isLegalMove(board, Position(1, 0), Position(1, 1)));
    }
}
