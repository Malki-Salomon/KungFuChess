#include "doctest.h"
#include "Queen.h"
#include "Board.h"

TEST_SUITE("Queen")
{
    TEST_CASE("Horizontal")
    {
        Board board;
        Queen queen(PieceColor::White);

        CHECK(queen.isValidMove(2, 2, 2, 6, board));
    }

    TEST_CASE("Vertical")
    {
        Board board;
        Queen queen(PieceColor::White);

        CHECK(queen.isValidMove(2, 2, 6, 2, board));
    }

    TEST_CASE("Diagonal")
    {
        Board board;
        Queen queen(PieceColor::White);

        CHECK(queen.isValidMove(2, 2, 6, 6, board));
    }

    TEST_CASE("Illegal")
    {
        Board board;
        Queen queen(PieceColor::White);

        CHECK_FALSE(queen.isValidMove(2, 2, 4, 5, board));
    }
}