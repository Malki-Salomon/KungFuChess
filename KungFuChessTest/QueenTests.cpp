#include "doctest.h"
#include "Queen.h"
#include "Board.h"

TEST_SUITE("Queen")
{
    TEST_CASE("Horizontal")
    {
        Board board;

        board.parse({
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . ."
            });

        Queen queen(PieceColor::White);

        CHECK(queen.isValidMove(2, 2, 2, 6, board));
    }

    TEST_CASE("Vertical")
    {
        Board board;

        board.parse({
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . ."
            });

        Queen queen(PieceColor::White);

        CHECK(queen.isValidMove(2, 2, 6, 2, board));
    }

    TEST_CASE("Diagonal")
    {
        Board board;

        board.parse({
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . ."
            });

        Queen queen(PieceColor::White);

        CHECK(queen.isValidMove(2, 2, 6, 6, board));
    }

    TEST_CASE("Illegal")
    {
        Board board;

        board.parse({
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . ."
            });

        Queen queen(PieceColor::White);

        CHECK_FALSE(queen.isValidMove(2, 2, 4, 5, board));
    }

    TEST_CASE("Blocked queen")
    {
        Board board;

        board.parse({
            ". . . .",
            ". wQ bP .",
            ". . . ."
            });

        Queen queen(PieceColor::White);

        CHECK_FALSE(
            queen.isValidMove(1, 1, 1, 3, board)
        );
    }
}