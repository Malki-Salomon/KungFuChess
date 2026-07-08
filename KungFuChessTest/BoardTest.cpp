#include "doctest.h"
#include "Board.h"
#include <vector>

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

TEST_CASE("Unknown token")
{
    Board board;

    std::vector<std::string> input =
    {
        "wK XX ."
    };

    board.parse(input);

    CHECK_FALSE(board.validate());
    CHECK(board.getError() == "ERROR UNKNOWN_TOKEN");
}

TEST_CASE("Rows with different lengths")
{
    Board board;

    std::vector<std::string> input =
    {
        "wK . .",
        ". bK"
    };

    board.parse(input);

    CHECK_FALSE(board.validate());
    CHECK(board.getError() == "ERROR ROW_WIDTH_MISMATCH");
}

TEST_CASE("Read tile")
{
    Board board;

    std::vector<std::string> input =
    {
        "wK .",
        ". bK"
    };

    board.parse(input);

    CHECK(board.getTile(0, 0) == "wK");
    CHECK(board.getTile(1, 1) == "bK");
}