#include "AppTest.h"
#include "doctest.h"
#include "App.h"
#include "Board.h"
#include <vector>
#include <string>

//TEST_CASE("Testing clicking multiple pieces - selection replacement")
//{
//    App app;
//
//    std::vector<std::string> inputLines =
//    {
//        " Board:",
//        "wR . wK",
//        ". . .",
//        "Commands:",
//        "click 50 50",
//        "click 250 50",
//        "click 250 150",
//        "wait 1000",
//        "print board"
//    };
//
//    app.parseLoad(inputLines);
//
//    const Board& board = app.getGame().getBoard();
//
//    CHECK(board.getPieceType(Position(0, 0)) == PieceType::Rook);
//    CHECK(board.getPieceType(Position(1, 2)) == PieceType::King);
//    CHECK(board.isEmpty(Position(0, 2)));
//}

//TEST_CASE("Testing sequential moves and game board state") {
//    App app;
//
//    // הגדרת הלוח כפי שהופיע בקלט
//    std::vector<std::string> inputLines = {
//        " Board:",
//        "wR . bK",
//        "bR . .",
//        "Commands:",
//        "click 50 50",   // בחירת wR ב-(0,0)
//        "click 250 50",  // תנועה ל-(0,2)
//        "wait 2000",     // המתנה לסיום התנועה הראשונה
//        "click 50 150",  // בחירת bR ב-(1,0)
//        "click 150 150", // תנועה ל-(1,1)
//        "wait 1000",     // המתנה לסיום התנועה השנייה
//        "print board"
//    };
//
//    app.parseLoad(inputLines);
//
//    const Board& board = app.getGame().getBoard();
//
//    // בדיקה 1: האם ה-wR הגיע ל-(0,2)?
//    CHECK(board.getPieceType(Position(0, 2)) == PieceType::Rook);
//    CHECK(board.getPieceColor(Position(0, 2)) == PieceColor::White);
//
//    // בדיקה 2: האם ה-bR הגיע ל-(1,1)?
//    CHECK(board.getPieceType(Position(1, 0)) == PieceType::Rook);
//    CHECK(board.getPieceColor(Position(1, 0)) == PieceColor::Black);
//
//    // בדיקה 3: האם המיקומים המקוריים ריקים?
//    CHECK(board.isEmpty(Position(0, 0)) == true);
//    CHECK(board.isEmpty(Position(1, 1)) == true);
//}

TEST_CASE("Testing King one-step valid move") {
    App app;

    // הגדרת הלוח והפקודות
    std::vector<std::string> inputLines = {
        " Board:",
        "wK . .",
        ". . .",
        ". . .",
        "Commands:",
        "click 50 50",    // בחירת wK ב-(0,0)
        "click 150 150",  // תנועה ל-(1,1)
        "wait 1000",      // המתנה לסיום המהלך
        "print board"
    };

    app.parseLoad(inputLines);
	app.run();

    // בדיקת הלוח לאחר המהלך
    const Board& board = app.getGame().getBoard();

    // בדיקה: האם ה-wK אכן הגיע ל-(1,1)?
    CHECK(board.getPieceType(Position(1, 1)) == PieceType::King);
    CHECK(board.getPieceColor(Position(1, 1)) == PieceColor::White);

    // בדיקה: האם המיקום המקורי ריק?
    CHECK(board.isEmpty(Position(0, 0)) == true);
}
