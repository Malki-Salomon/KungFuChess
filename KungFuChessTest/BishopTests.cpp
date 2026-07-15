#include "doctest.h"
#include "Bishop.h"
#include "Board.h"
#include "TextBoardConvert.h"
#include "Position.h" 

TEST_SUITE("Bishop")
{
    TEST_CASE("Diagonal")
    {
        Board board;

        TextBoardConvert convert({
             ". . . . . .",
             ". . . . . .",
             ". . . . . .",
             ". . . . . .",
             ". . . . . .",
             ". . . . . ."
            });

        convert.Convert(board);

        Bishop bishop(PieceColor::White, Position(2,2), PieceType::Bishop);

        CHECK(bishop.isValidMove(Position(2, 2), Position(5, 5), board));
        CHECK(bishop.isValidMove(Position(5, 5), Position(2, 2), board));
    }

    TEST_CASE("Blocked diagonal")
    {
        Board board;

        TextBoardConvert convert({
            ". . . . . .",
            ". . . . . .",
            ". . wB . . .",
            ". . . wP . .",
            ". . . . . .",
            ". . . . . ."
            });

        convert.Convert(board);

        Bishop bishop(PieceColor::White, Position(2, 2), PieceType::Bishop);

        CHECK(
            bishop.isValidMove(Position(2, 2), Position(4, 4), board)
        );
    }

    TEST_CASE("Horizontal")
    {
        Board board;

        TextBoardConvert convert({
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . ."
            });
        convert.Convert(board);

        Bishop bishop(PieceColor::White, Position(2, 2), PieceType::Bishop);

        CHECK_FALSE(
            bishop.isValidMove(Position(2, 2), Position(2, 5), board)
        );
    }

    TEST_CASE("Vertical")
    {
        Board board;

        TextBoardConvert convert({
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . ."
            });
        convert.Convert(board);

        Bishop bishop(PieceColor::White, Position(2, 2), PieceType::Bishop);

        CHECK_FALSE(
            bishop.isValidMove(Position(2, 2), Position(5, 2), board)
        );
    }

    TEST_CASE("Same square")
    {
        Board board;

        TextBoardConvert convert({
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . .",
            ". . . . . ."
            });
        convert.Convert(board);

        Bishop bishop(PieceColor::White, Position(2, 2), PieceType::Bishop);

        CHECK_FALSE(
            bishop.isValidMove(Position(2, 2), Position(2, 2), board)
        );
    }
}