#include "doctest.h"
#include "PieceRegistry.h"

TEST_SUITE("PieceRegistry::tryParseSymbol")
{
    TEST_CASE("round-trips every real piece type/color through its symbol")
    {
        for (PieceType type : { PieceType::King, PieceType::Queen, PieceType::Rook,
                                 PieceType::Bishop, PieceType::Knight, PieceType::Pawn })
        {
            const PieceMetadata& meta = PieceRegistry::getMetadata(type);

            PieceType parsedType;
            PieceColor parsedColor;

            CHECK(PieceRegistry::tryParseSymbol(meta.whiteSymbol, parsedType, parsedColor));
            CHECK(parsedType == type);
            CHECK(parsedColor == PieceColor::White);

            CHECK(PieceRegistry::tryParseSymbol(meta.blackSymbol, parsedType, parsedColor));
            CHECK(parsedType == type);
            CHECK(parsedColor == PieceColor::Black);
        }
    }

    TEST_CASE("rejects the empty-cell symbol")
    {
        PieceType type;
        PieceColor color;
        CHECK_FALSE(PieceRegistry::tryParseSymbol(".", type, color));
    }

    // getMetadata(Empty) used to run off the end of the function without
    // returning - undefined behavior (warning C4715). It now has one
    // defined answer: all-blank metadata.
    TEST_CASE("getMetadata(Empty) returns blank metadata instead of being undefined")
    {
        const PieceMetadata& meta = PieceRegistry::getMetadata(PieceType::Empty);

        CHECK(meta.name.empty());
        CHECK(meta.whiteSymbol.empty());
        CHECK(meta.blackSymbol.empty());
        CHECK(meta.whiteImagePath.empty());
        CHECK(meta.blackImagePath.empty());
    }

    TEST_CASE("getMetadata(Empty) is stable across calls")
    {
        // The returned reference must have static storage duration - a
        // reference to a temporary would dangle the moment it was used.
        const PieceMetadata& first = PieceRegistry::getMetadata(PieceType::Empty);
        const PieceMetadata& second = PieceRegistry::getMetadata(PieceType::Empty);

        CHECK(&first == &second);
    }

    // The blank result must be distinguishable from every real piece, so a
    // caller that missed its emptiness check cannot mistake it for one.
    TEST_CASE("blank metadata collides with no real piece's symbols")
    {
        const PieceMetadata& empty = PieceRegistry::getMetadata(PieceType::Empty);

        for (PieceType type : { PieceType::King, PieceType::Queen, PieceType::Rook,
                                 PieceType::Bishop, PieceType::Knight, PieceType::Pawn })
        {
            const PieceMetadata& meta = PieceRegistry::getMetadata(type);

            CHECK_FALSE(meta.name.empty());
            CHECK_FALSE(meta.whiteSymbol.empty());
            CHECK_FALSE(meta.blackSymbol.empty());

            CHECK(meta.whiteSymbol != empty.whiteSymbol);
            CHECK(meta.blackSymbol != empty.blackSymbol);
        }
    }

    // Empty is absent from the symbol table, so its blank symbol must not
    // have become a parseable one.
    TEST_CASE("blank symbol does not parse back to a piece")
    {
        PieceType type;
        PieceColor color;
        CHECK_FALSE(PieceRegistry::tryParseSymbol("", type, color));
    }

    TEST_CASE("rejects an unrecognized symbol without crashing")
    {
        PieceType type;
        PieceColor color;
        CHECK_FALSE(PieceRegistry::tryParseSymbol("zz", type, color));
        CHECK_FALSE(PieceRegistry::tryParseSymbol("", type, color));
    }
}
