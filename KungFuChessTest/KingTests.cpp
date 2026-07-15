#include "doctest.h"
#include "King.h"
#include "Board.h"
#include "Position.h" 
TEST_SUITE("King")
{
    TEST_CASE("Legal moves")
    {
        Board board;
        King king(PieceColor::White, Position(3, 3), PieceType::King);

        CHECK(king.isValidMove(Position(3, 3), Position(3, 4), board));
        CHECK(king.isValidMove(Position(3, 3), Position(4, 3), board));
        CHECK(king.isValidMove(Position(3, 3), Position(4, 4), board));
        CHECK(king.isValidMove(Position(3, 3), Position(2, 2), board));
    }

    TEST_CASE("Illegal moves")
    {
        Board board;
        King king(PieceColor::White, Position(3, 3), PieceType::King);

        CHECK_FALSE(king.isValidMove(Position(3, 3), Position(5, 3), board));
        CHECK_FALSE(king.isValidMove(Position(3, 3), Position(3, 5), board));
        CHECK_FALSE(king.isValidMove(Position(3, 3), Position(5, 5), board));
    }

    TEST_CASE("Same square")
    {
        Board board;
        King king(PieceColor::White, Position(3, 3), PieceType::King);

        CHECK_FALSE(king.isValidMove(Position(3, 3), Position(3, 3), board));
    }
}