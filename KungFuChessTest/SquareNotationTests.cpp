#include "doctest.h"

#include "SquareNotation.h" // KungFuChessProtocol - the single source of truth

#include <string>

TEST_SUITE("Protocol::SquareNotation")
{
    TEST_CASE("pins the board orientation at the four corners")
    {
        // row 0 is black's back rank (rank 8), row 7 is white's (rank 1);
        // col 0 is file 'a'. Getting any of these backwards would put every
        // click on the wrong square, so they are pinned explicitly rather
        // than only checked for self-consistency by the round-trip below.
        CHECK(Protocol::SquareNotation::toNotation(0, 0) == "a8");
        CHECK(Protocol::SquareNotation::toNotation(0, 7) == "h8");
        CHECK(Protocol::SquareNotation::toNotation(7, 0) == "a1");
        CHECK(Protocol::SquareNotation::toNotation(7, 7) == "h1");
    }

    TEST_CASE("parses the four corners back to the same cells")
    {
        Protocol::Square square{};

        REQUIRE(Protocol::SquareNotation::tryParse("a8", square));
        CHECK(square.row == 0);
        CHECK(square.col == 0);

        REQUIRE(Protocol::SquareNotation::tryParse("h8", square));
        CHECK(square.row == 0);
        CHECK(square.col == 7);

        REQUIRE(Protocol::SquareNotation::tryParse("a1", square));
        CHECK(square.row == 7);
        CHECK(square.col == 0);

        REQUIRE(Protocol::SquareNotation::tryParse("h1", square));
        CHECK(square.row == 7);
        CHECK(square.col == 7);
    }

    TEST_CASE("matches the starting position's known squares")
    {
        // White's e-pawn starts on e2 - row 6, col 4 in the wire grid -
        // and moves to e4, two rows up.
        CHECK(Protocol::SquareNotation::toNotation(6, 4) == "e2");
        CHECK(Protocol::SquareNotation::toNotation(4, 4) == "e4");

        // Both kings start on the e-file.
        CHECK(Protocol::SquareNotation::toNotation(7, 4) == "e1");
        CHECK(Protocol::SquareNotation::toNotation(0, 4) == "e8");
    }

    // The core guarantee of having one implementation: for every square on
    // the board, coordinates -> notation -> coordinates lands exactly where
    // it started.
    TEST_CASE("all 64 squares round-trip coordinates -> notation -> coordinates")
    {
        for (int row = 0; row < 8; ++row)
        {
            for (int col = 0; col < 8; ++col)
            {
                std::string notation = Protocol::SquareNotation::toNotation(row, col);
                REQUIRE_FALSE(notation.empty());

                Protocol::Square square{};
                REQUIRE(Protocol::SquareNotation::tryParse(notation, square));

                CHECK(square.row == row);
                CHECK(square.col == col);
            }
        }
    }

    TEST_CASE("all 64 notations round-trip notation -> coordinates -> notation")
    {
        for (char file = 'a'; file <= 'h'; ++file)
        {
            for (char rank = '1'; rank <= '8'; ++rank)
            {
                std::string expected = std::string(1, file) + std::string(1, rank);

                Protocol::Square square{};
                REQUIRE(Protocol::SquareNotation::tryParse(expected, square));

                CHECK(Protocol::SquareNotation::toNotation(square.row, square.col) == expected);
            }
        }
    }

    TEST_CASE("tryParse rejects invalid notation")
    {
        Protocol::Square square{};

        CHECK_FALSE(Protocol::SquareNotation::tryParse("z9", square));   // bad file AND rank
        CHECK_FALSE(Protocol::SquareNotation::tryParse("a0", square));   // rank below range
        CHECK_FALSE(Protocol::SquareNotation::tryParse("a9", square));   // rank above range
        CHECK_FALSE(Protocol::SquareNotation::tryParse("i1", square));   // file just past 'h'
        CHECK_FALSE(Protocol::SquareNotation::tryParse("", square));     // empty
        CHECK_FALSE(Protocol::SquareNotation::tryParse("abc", square));  // too long
        CHECK_FALSE(Protocol::SquareNotation::tryParse("a", square));    // too short
        CHECK_FALSE(Protocol::SquareNotation::tryParse("8a", square));   // reversed
        CHECK_FALSE(Protocol::SquareNotation::tryParse("  ", square));   // whitespace
    }

    TEST_CASE("tryParse leaves the out-param untouched when rejecting")
    {
        Protocol::Square square{ 3, 5 };

        CHECK_FALSE(Protocol::SquareNotation::tryParse("z9", square));
        CHECK(square.row == 3);
        CHECK(square.col == 5);
    }

    TEST_CASE("tryParse accepts an uppercase file")
    {
        Protocol::Square square{};

        REQUIRE(Protocol::SquareNotation::tryParse("E2", square));
        CHECK(square.row == 6);
        CHECK(square.col == 4);
    }

    // toNotation has no bool channel, so an out-of-range cell reports
    // itself as an empty string rather than composing text like "i9" that
    // would then travel over the wire.
    TEST_CASE("toNotation returns empty for cells outside the board")
    {
        CHECK(Protocol::SquareNotation::toNotation(-1, 0).empty());
        CHECK(Protocol::SquareNotation::toNotation(0, -1).empty());
        CHECK(Protocol::SquareNotation::toNotation(8, 0).empty());
        CHECK(Protocol::SquareNotation::toNotation(0, 8).empty());
        CHECK(Protocol::SquareNotation::toNotation(100, 100).empty());
    }
}
