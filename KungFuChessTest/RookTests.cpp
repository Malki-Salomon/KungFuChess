#include "doctest.h"
#include "Rook.h"
#include "Board.h"

TEST_SUITE("Rook")
{
    TEST_CASE("Horizontal")
    {
        Board board;
        Rook rook(PieceColor::White);

        CHECK(rook.isValidMove(3, 3, 3, 7, board));
    }

    TEST_CASE("Vertical")
    {
        Board board;
        Rook rook(PieceColor::White);

        CHECK(rook.isValidMove(3, 3, 0, 3, board));
    }

    TEST_CASE("Diagonal")
    {
        Board board;
        Rook rook(PieceColor::White);

        CHECK_FALSE(rook.isValidMove(3, 3, 6, 6, board));
    }

    TEST_CASE("Same square")
    {
        Board board;
        Rook rook(PieceColor::White);

        CHECK_FALSE(rook.isValidMove(3, 3, 3, 3, board));
    }
}