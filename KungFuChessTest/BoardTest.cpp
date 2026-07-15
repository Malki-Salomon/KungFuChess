#include "doctest.h"
#include "Board.h"
#include "TextBoardConvert.h"
#include "Position.h"

TEST_SUITE("Board")
{
    TEST_CASE("isInside")
    {
        Board board;

        TextBoardConvert converter({
            "wK . .",
            ". . ."
        });

        CHECK(converter.Convert(board));

        CHECK(board.isInside(Position(0, 0)));
        CHECK(board.isInside(Position(1, 2)));

        CHECK_FALSE(board.isInside(Position(-1, 0)));
        CHECK_FALSE(board.isInside(Position(2, 0)));
        CHECK_FALSE(board.isInside(Position(0, 3)));
    }

    TEST_CASE("isEmpty")
    {
        Board board;

        TextBoardConvert converter({
            "wK . ."
        });

        CHECK(converter.Convert(board));

        CHECK_FALSE(board.isEmpty(Position(0, 0)));
        CHECK(board.isEmpty(Position(0, 1)));
        CHECK(board.isEmpty(Position(0, 2)));
    }

    TEST_CASE("Piece color")
    {
        Board board;

        TextBoardConvert converter({
            "wK bQ ."
        });

        CHECK(converter.Convert(board));

        CHECK(board.getPieceColor(Position(0, 0)) == PieceColor::White);
        CHECK(board.getPieceColor(Position(0, 1)) == PieceColor::Black);
    }

    TEST_CASE("Piece type")
    {
        Board board;

        TextBoardConvert converter({
            "wK bR ."
        });

        CHECK(converter.Convert(board));

        CHECK(board.getPieceType(Position(0, 0)) == PieceType::King);
        CHECK(board.getPieceType(Position(0, 1)) == PieceType::Rook);
    }

    TEST_CASE("Move piece")
    {
        Board board;

        TextBoardConvert converter({
            "wR . ."
        });

        CHECK(converter.Convert(board));

        board.movePiece(Position(0, 0), Position(0, 2));

        CHECK(board.isEmpty(Position(0, 0)));
        CHECK(board.getPieceType(Position(0, 2)) == PieceType::Rook);
        CHECK(board.getPieceColor(Position(0, 2)) == PieceColor::White);
    }
}