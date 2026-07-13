#include "doctest.h"
#include "Queen.h"
#include "Board.h"
#include "TextBoardConvert.h"
#include "Position.h" 
TEST_SUITE("Queen")
{
    TEST_CASE("Horizontal")
    {
        Board board;

        TextBoardConvert convert({
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . ."
            });
        convert.Convert(board);

        Queen queen(PieceColor::White);

        CHECK(queen.isValidMove(Position(2, 2), Position(2, 6), board));
    }

    TEST_CASE("Vertical")
    {
        Board board;

        TextBoardConvert convert({
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . ."
            });
        convert.Convert(board);

        Queen queen(PieceColor::White);

        CHECK(queen.isValidMove(Position(2, 2), Position(6, 2), board));
    }

    TEST_CASE("Diagonal")
    {
        Board board;

        TextBoardConvert convert({
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . ."
            });
        convert.Convert(board);

        Queen queen(PieceColor::White);

        CHECK(queen.isValidMove(Position(2, 2), Position(6, 6), board));
    }

    TEST_CASE("Illegal")
    {
        Board board;

        TextBoardConvert convert({
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . .",
            ". . . . . . ."
            });
        convert.Convert(board);

        Queen queen(PieceColor::White);

        CHECK_FALSE(queen.isValidMove(Position(2, 2), Position(4, 5), board));
    }

    TEST_CASE("Blocked queen")
    {
        Board board;

        TextBoardConvert convert({
            ". . . .",
            ". wQ bP .",
            ". . . ."
            });
        convert.Convert(board);

        Queen queen(PieceColor::White);

        CHECK(
            queen.isValidMove(Position(1, 1), Position(1, 3), board)
        );
    }
}