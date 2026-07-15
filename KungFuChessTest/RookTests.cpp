#include "doctest.h"
#include "Rook.h"
#include "Board.h"
#include "TextBoardConvert.h"
#include "Position.h"

TEST_SUITE("Rook")
{
    TEST_CASE("Horizontal")
    {
        Board board;

        TextBoardConvert convert({
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . ."
            });
        convert.Convert(board);
        Rook rook(PieceColor::White, Position(3, 3), PieceType::Rook);

        CHECK(rook.isValidMove(Position(3, 3), Position(3, 7), board));
    }

    TEST_CASE("Vertical")
    {
        Board board;

        TextBoardConvert convert({
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . ."
            });
        convert.Convert(board);

        Rook rook(PieceColor::White, Position(3, 3), PieceType::Rook);

        CHECK(rook.isValidMove(Position(3, 3), Position(0, 3), board));
    }

    TEST_CASE("Diagonal")
    {
        Board board;

        TextBoardConvert convert({
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . ."
            });
        convert.Convert(board);

        Rook rook(PieceColor::White, Position(3, 3), PieceType::Rook);

        CHECK_FALSE(rook.isValidMove(Position(3, 3), Position(6, 6), board));
    }

    TEST_CASE("Same square")
    {
        Board board;

        TextBoardConvert convert({
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . .",
            ". . . . . . . ."
            });
        convert.Convert(board);

        Rook rook(PieceColor::White, Position(3, 3), PieceType::Rook);

        CHECK_FALSE(rook.isValidMove(Position(3, 3), Position(3, 3), board));
    }

    TEST_CASE("Blocked horizontal")
    {
        Board board;

        TextBoardConvert convert({
            ". . . . .",
            ". . . . .",
            ". . wR bP .",
            ". . . . ."
            });
        convert.Convert(board);

        Rook rook(PieceColor::White, Position(2, 2), PieceType::Rook);

        CHECK(
            rook.isValidMove(Position(2, 2), Position(2, 4), board)
        );
    }
}