#include "doctest.h"
#include "TextBoardConvert.h"
#include "Board.h"
#include <vector>
#include <string>

using namespace std;

TEST_CASE("Testing TextBoardConvert Class") {
    Board board;

    SUBCASE("1. Happy Paths") {

        SUBCASE("Valid standard board conversion") {
            vector<string> input = {
                "wR wN wB wQ wK wB wN wR",
                "wP wP wP wP wP wP wP wP",
                ".  .  .  .  .  .  .  .",
                "bR bN bB bQ bK bB bN bR"
            };
            TextBoardConvert converter(input);
            CHECK(converter.Convert(board) == true);
        }

        SUBCASE("All empty spaces") {
            vector<string> input = {
                ". .",
                ". ."
            };
            TextBoardConvert converter(input);
            CHECK(converter.Convert(board) == true);
        }

        SUBCASE("Multiple spaces between tokens should be handled correctly") {
            vector<string> input = {
                "wP    .    bP",
                ".     .     ."
            };
            TextBoardConvert converter(input);
            CHECK(converter.Convert(board) == true);
        }
    }

    SUBCASE("2. Edge Cases - Invalid Tokens") {

        SUBCASE("Invalid token length (too long)") {
            vector<string> input = { "wRR ." };
            TextBoardConvert converter(input);
            CHECK(converter.Convert(board) == false);
        }

        SUBCASE("Invalid piece color (not w or b)") {
            vector<string> input = { "xP ." };
            TextBoardConvert converter(input);
            CHECK(converter.Convert(board) == false);
        }

        SUBCASE("Invalid piece type (unknown character)") {
            vector<string> input = { "wZ ." };
            TextBoardConvert converter(input);
            CHECK(converter.Convert(board) == false);
        }

        SUBCASE("Lowercase piece type should fail") {
            vector<string> input = { "wp ." };
            TextBoardConvert converter(input);
            CHECK(converter.Convert(board) == false);
        }
    }

    SUBCASE("3. Edge Cases - Structure and Boundaries") {

        SUBCASE("Empty input vector") {
            vector<string> input = {};
            TextBoardConvert converter(input);
            CHECK(converter.Convert(board) == false);
        }

        SUBCASE("Non-uniform row lengths") {
            vector<string> input = {
                "wP .",
                "wP . bP"
            };
            TextBoardConvert converter(input);
            CHECK(converter.Convert(board) == false);
        }
    }
}