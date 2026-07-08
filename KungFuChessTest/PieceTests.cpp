#include "doctest.h"

#include "Piece.h"
#include "Board.h"

class MockPiece : public Piece
{
public:
    MockPiece(PieceColor color)
        : Piece(color)
    {
    }

    PieceType getType() const override
    {
        return PieceType::Pawn;
    }

    bool isValidMove(
        int,
        int,
        int,
        int,
        const Board&) const override
    {
        return true;
    }
};

TEST_SUITE("Piece")
{
    TEST_CASE("White piece color")
    {
        MockPiece piece(PieceColor::White);

        CHECK(piece.getColor() == PieceColor::White);
    }

    TEST_CASE("Black piece color")
    {
        MockPiece piece(PieceColor::Black);

        CHECK(piece.getColor() == PieceColor::Black);
    }

    TEST_CASE("Piece type")
    {
        MockPiece piece(PieceColor::White);

        CHECK(piece.getType() == PieceType::Pawn);
    }

    TEST_CASE("Virtual function through base pointer")
    {
        MockPiece mock(PieceColor::White);

        Piece* piece = &mock;

        CHECK(piece->getType() == PieceType::Pawn);
    }

    TEST_CASE("Virtual isValidMove")
    {
        Board board;
        MockPiece mock(PieceColor::White);

        Piece* piece = &mock;

        CHECK(piece->isValidMove(0, 0, 7, 7, board));
    }

    TEST_CASE("Reference to base class")
    {
        MockPiece mock(PieceColor::Black);

        Piece& piece = mock;

        CHECK(piece.getColor() == PieceColor::Black);
        CHECK(piece.getType() == PieceType::Pawn);
    }

    TEST_CASE("None piece color")
    {
        MockPiece piece(PieceColor::None);

        CHECK(piece.getColor() == PieceColor::None);
    }
}