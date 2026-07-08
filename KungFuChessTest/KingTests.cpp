#include "doctest.h"
#include "King.h"
#include "Board.h"

TEST_SUITE("King")
{
    TEST_CASE("Legal moves")
    {
        Board board;
        King king(PieceColor::White);

        CHECK(king.isValidMove(3, 3, 3, 4, board));
        CHECK(king.isValidMove(3, 3, 4, 3, board));
        CHECK(king.isValidMove(3, 3, 4, 4, board));
        CHECK(king.isValidMove(3, 3, 2, 2, board));
    }

    TEST_CASE("Illegal moves")
    {
        Board board;
        King king(PieceColor::White);

        CHECK_FALSE(king.isValidMove(3, 3, 5, 3, board));
        CHECK_FALSE(king.isValidMove(3, 3, 3, 5, board));
        CHECK_FALSE(king.isValidMove(3, 3, 5, 5, board));
    }

    TEST_CASE("Same square")
    {
        Board board;
        King king(PieceColor::White);

        CHECK_FALSE(king.isValidMove(3, 3, 3, 3, board));
    }
}