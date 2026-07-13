#include "doctest.h"
#include "Knight.h"
#include "Board.h"
#include "Position.h" 
TEST_SUITE("Knight")
{
    TEST_CASE("Legal moves")
    {
        Board board;
        Knight knight(PieceColor::White);

        CHECK(knight.isValidMove(Position(3, 3), Position(5, 4), board));
        CHECK(knight.isValidMove(Position(3, 3), Position(5, 2), board));
        CHECK(knight.isValidMove(Position(3, 3), Position(1, 4), board));
        CHECK(knight.isValidMove(Position(3, 3), Position(1, 2), board));
    }

    TEST_CASE("Illegal moves")
    {
        Board board;
        Knight knight(PieceColor::White);

        CHECK_FALSE(knight.isValidMove(Position(3, 3), Position(4, 4), board));
        CHECK_FALSE(knight.isValidMove(Position(3, 3), Position(3, 5), board));
        CHECK_FALSE(knight.isValidMove(Position(3, 3), Position(5, 5), board));
    }

    TEST_CASE("Same square")
    {
        Board board;
        Knight knight(PieceColor::White);

        CHECK_FALSE(knight.isValidMove(Position(3, 3), Position(3, 3), board));
    }
}