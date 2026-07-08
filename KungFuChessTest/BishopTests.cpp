#include "doctest.h"
#include "Bishop.h"
#include "Board.h"

TEST_SUITE("Bishop")
{
    TEST_CASE("Diagonal")
    {
        Board board;

        board.parse({
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . ."
            });

        Bishop bishop(PieceColor::White);

        CHECK(bishop.isValidMove(2, 2, 5, 5, board));
        CHECK(bishop.isValidMove(5, 5, 2, 2, board));
    }

    TEST_CASE("Blocked diagonal")
    {
        Board board;

        board.parse({
            ". . . . . .",
            ". . . . . .",
            ". . wB . . .",
            ". . . wP . .",
            ". . . . . .",
            ". . . . . ."
            });

        Bishop bishop(PieceColor::White);

        CHECK_FALSE(
            bishop.isValidMove(2, 2, 4, 4, board)
        );
    }

    TEST_CASE("Horizontal")
    {
        Board board;

        board.parse({
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . ."
            });

        Bishop bishop(PieceColor::White);

        CHECK_FALSE(
            bishop.isValidMove(2, 2, 2, 5, board)
        );
    }

    TEST_CASE("Vertical")
    {
        Board board;

        board.parse({
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . ."
            });

        Bishop bishop(PieceColor::White);

        CHECK_FALSE(
            bishop.isValidMove(2, 2, 5, 2, board)
        );
    }

    TEST_CASE("Same square")
    {
        Board board;

        board.parse({
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . ."
            });

        Bishop bishop(PieceColor::White);

        CHECK_FALSE(
            bishop.isValidMove(2, 2, 2, 2, board)
        );
    }
}