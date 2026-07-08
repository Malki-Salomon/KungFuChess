#include "doctest.h"
#include "Board.h"

TEST_SUITE("Board")
{
    TEST_CASE("Valid board")
    {
        Board board;

        std::vector<std::string> input =
        {
            "wK . .",
            ". bK .",
            ". . ."
        };

        board.parse(input);

        CHECK(board.validate());
        CHECK(board.getRows() == 3);
        CHECK(board.getCols() == 3);
    }

    TEST_CASE("Empty board")
    {
        Board board;

        std::vector<std::string> input;

        board.parse(input);

        CHECK(board.validate());
        CHECK(board.getRows() == 0);
        CHECK(board.getCols() == 0);
    }

    TEST_CASE("Invalid token")
    {
        Board board;

        std::vector<std::string> input =
        {
            "wK XX ."
        };

        board.parse(input);

        CHECK_FALSE(board.validate());
    }

    TEST_CASE("Different row lengths")
    {
        Board board;

        std::vector<std::string> input =
        {
            "wK . .",
            ". ."
        };

        board.parse(input);

        CHECK_FALSE(board.validate());
    }

    TEST_CASE("isEmpty")
    {
        Board board;

        board.parse(
            {
                "wK .",
                ". bR"
            });

        CHECK_FALSE(board.isEmpty(0, 0));
        CHECK(board.isEmpty(0, 1));
    }

    TEST_CASE("Piece type")
    {
        Board board;

        board.parse(
            {
                "wQ bR",
                "wB wN"
            });

        CHECK(board.getPieceType(0, 0) == PieceType::Queen);
        CHECK(board.getPieceType(0, 1) == PieceType::Rook);
        CHECK(board.getPieceType(1, 0) == PieceType::Bishop);
        CHECK(board.getPieceType(1, 1) == PieceType::Knight);
    }

    TEST_CASE("Piece color")
    {
        Board board;

        board.parse(
            {
                "wQ bR"
            });

        CHECK(board.getPieceColor(0, 0) == PieceColor::White);
        CHECK(board.getPieceColor(0, 1) == PieceColor::Black);
    }

    TEST_CASE("Move piece")
    {
        Board board;

        board.parse(
            {
                "wR .",
                ". ."
            });

        board.movePiece(0, 0, 1, 1);

        CHECK(board.isEmpty(0, 0));
        CHECK(board.getPieceType(1, 1) == PieceType::Rook);
    }
}