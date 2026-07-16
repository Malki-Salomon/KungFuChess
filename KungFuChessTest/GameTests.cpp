#include "doctest.h"
#include "App.h"
#include "Board.h"
#include <vector>
#include <string>

TEST_CASE("Testing Board initialization - No Error Expected") {
    App app;
    // הקלט התקין כפי שציינת
    std::vector<std::string> inputLines = {
        " Board:",
        "wK . . bK",
        ". . . .",
        "wR . . bR",
        "Commands:",
        "print board"
    };

    // הרצת הסימולציה
    app.parseLoad(inputLines);

    // גישה ללוח לבדיקה
    const Board& board = app.getGame().getBoard();

    // 1. וודא שאין הודעת שגיאה בלוח
    CHECK(board.getError().empty());

    // 2. בדיקת המיקומים הספציפיים כדי לוודא טעינה נכונה של הכלים
    CHECK(board.getPieceType(Position(0, 0)) == PieceType::King);
    CHECK(board.getPieceType(Position(0, 3)) == PieceType::King);
    CHECK(board.getPieceType(Position(2, 0)) == PieceType::Rook);
    CHECK(board.getPieceType(Position(2, 3)) == PieceType::Rook);
}

//TEST_CASE("Testing clicking multiple pieces - selection replacement") {
//    Game game;
//
//    // הגדרת הלוח כפי שמופיע בקלט
//    std::vector<std::string> inputLines = {
//        " Board:",
//        "wR . wK",
//        ". . .",
//        "Commands:",
//        "click 50 50",    // לחיצה ראשונה: בוחר את ה-wR ב-(0,0)
//        "click 250 50",   // לחיצה שנייה: האם הוא מבטל את הבחירה ב-wR ומנסה לבחור ב-(0,2)?
//        // במימוש שלך - זו אמורה להיות בחירת ה-wK ב-(0,2)
//"click 250 150",  // תנועה של ה-wK ל-(1,2)
//"wait 1000",      // המתנה לסיום המהלך
//"print board"
//    };
//
//    game.runSimulation(inputLines);
//
//    const Board& board = game.getBoard();
//
//    // בדיקה: ה-wR אמור להישאר במקום המקורי (0,0) כי הוא לא זז
//    CHECK(board.getPieceType(Position(0, 0)) == PieceType::Rook);
//
//    // בדיקה: ה-wK אמור להיות במיקום החדש (1,2)
//    CHECK(board.getPieceType(Position(1, 2)) == PieceType::King);
//
//    // בדיקה: המיקום המקורי של ה-wK ב-(0,2) אמור להיות ריק
//    CHECK(board.isEmpty(Position(0, 2)) == true);
//}
//TEST_CASE("Testing sequential moves and game board state") {
//    Game game;
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
//    game.runSimulation(inputLines);
//
//    const Board& board = game.getBoard();
//
//    // בדיקה 1: האם ה-wR הגיע ל-(0,2)?
//    CHECK(board.getPieceType(Position(0, 2)) == PieceType::Rook);
//    CHECK(board.getPieceColor(Position(0, 2)) == PieceColor::White);
//
//    // בדיקה 2: האם ה-bR הגיע ל-(1,1)?
//    CHECK(board.getPieceType(Position(1, 1)) == PieceType::Rook);
//    CHECK(board.getPieceColor(Position(1, 1)) == PieceColor::Black);
//
//    // בדיקה 3: האם המיקומים המקוריים ריקים?
//    CHECK(board.isEmpty(Position(0, 0)) == true);
//    CHECK(board.isEmpty(Position(1, 0)) == true);
//}
//TEST_CASE("Testing King one-step valid move") {
//    Game game;
//
//    // הגדרת הלוח והפקודות
//    std::vector<std::string> inputLines = {
//        " Board:",
//        "wK . .",
//        ". . .",
//        ". . .",
//        "Commands:",
//        "click 50 50",    // בחירת wK ב-(0,0)
//        "click 150 150",  // תנועה ל-(1,1)
//        "wait 1000",      // המתנה לסיום המהלך
//        "print board"
//    };
//
//    game.runSimulation(inputLines);
//
//    // בדיקת הלוח לאחר המהלך
//    const Board& board = game.getBoard();
//
//    // בדיקה: האם ה-wK אכן הגיע ל-(1,1)?
//    CHECK(board.getPieceType(Position(1, 1)) == PieceType::King);
//    CHECK(board.getPieceColor(Position(1, 1)) == PieceColor::White);
//
//    // בדיקה: האם המיקום המקורי ריק?
//    CHECK(board.isEmpty(Position(0, 0)) == true);
//}
//TEST_CASE("Testing Rook movement and jump simulation") {
//    Game game;
//
//    // הגדרת הלוח והפקודות כפי שמופיע בקלט
//    std::vector<std::string> inputLines = {
//        " Board:",
//        "wR . .",
//        "Commands:",
//        "click 50 50",    // בחירת ה-wR ב-(0,0)
//        "click 250 50",   // תנועה ל-(0,2)
//        "wait 500",       // המתנה חלקית (התנועה בעיצומה)
//        "jump 50 50",     // ניסיון קפיצה (אמור להיכשל כי הכלי ב-moving)
//        "wait 1500",      // סיום התנועה
//        "print board"
//    };
//
//    // הרצת הסימולציה
//    game.runSimulation(inputLines);
//
//    // בדיקה לאחר סיום הסימולציה
//    const Board& board = game.getBoard();
//
//    // הציפייה: ה-wR אמור להשלים את התנועה ולהגיע ל-(0,2)
//    // הערה: ה-inputLines שלך מציין Expected: . . wR
//    // בלוח של 1x3, המיקום (0,2) הוא העמודה השלישית
//    CHECK(board.getPieceType(Position(0, 2)) == PieceType::Rook);
//    CHECK(board.getPieceColor(Position(0, 2)) == PieceColor::White);
//
//    // וודא שהמשבצת הראשונה ריקה
//    CHECK(board.isEmpty(Position(0, 0)) == true);
//}
//TEST_CASE("Testing Jump capture logic - Rook capturing Airborne King") {
//    Game game;
//
//    // הקלט כפי שצוין בטסט שלך
//    std::vector<std::string> inputLines = {
//        " Board:",
//        ". . . .",
//        "wK . . bR",
//        ". . . .",
//        "Commands:",
//        "jump 50 150",     // קפיצה של wK (במשבצת 1,0) - הופך ל-airborne
//        "wait 1000",       // המתנה שהקפיצה תתחיל/תתרחש
//        "click 350 150",   // בחירת ה-bR (במשבצת 1,3)
//        "click 50 150",    // תנועה של ה-bR למשבצת של ה-wK
//        "wait 3000",       // המתנה שהתנועה תסתיים
//        "print board"
//    };
//
//    // הרצת הסימולציה
//    game.runSimulation(inputLines);
//
//    // בדיקה לאחר סיום הסימולציה
//    const Board& board = game.getBoard();
//
//    // 1. ה-bR אמור להיות במיקום (1,0) כי הוא "אכל" את המיקום
//    // (ה-wK הוגן על עצמו ונשאר שם, או לפי הלוגיקה שביקשת שה-bR יופיע שם)
//    CHECK(board.getPieceType(Position(1, 0)) == PieceType::Rook);
//    CHECK(board.getPieceColor(Position(1, 0)) == PieceColor::Black);
//
//    // 2. המיקום המקורי של ה-bR (1,3) צריך להיות ריק
//    CHECK(board.isEmpty(Position(1, 3)) == true);
//}
//
//TEST_CASE("Testing Piece movement and jump capture logic") {
//    Game game;
//
//    // הגדרת הלוח כפי שמופיע בקלט שלך
//    std::vector<std::string> inputLines = {
//        " Board:",
//        ". . .",
//        "wK bR .",
//        ". . .",
//        "Commands:",
//        "click 150 150", // בחירת ה-bR שנמצא ב-(1,1)
//        "click 50 150",  // תנועה לעבר (1,0) - המיקום של ה-wK
//        "wait 1000",     // המתנה לסיום התנועה של ה-bR
//        "jump 50 150",   // ה-wK קופץ (הופך ל-airborne) ב-(1,0)
//        "print board"
//    };
//
//    // הרצת הסימולציה
//    game.runSimulation(inputLines);
//
//    // בדיקה לאחר סיום הסימולציה
//    const Board& board = game.getBoard();
//
//    // לפי הציפייה שלך: ה-bR אמור להיות ב-(0,0) או המיקום שהוגדר בציפייה
//    // הערה: שים לב שבתרחיש שלך ה-bR עבר למיקום שבו היה ה-wK
//    CHECK(board.getPieceType(Position(1, 0)) == PieceType::Rook);
//    CHECK(board.getPieceColor(Position(1, 0)) == PieceColor::Black);
//}
//TEST_CASE("Testing Jump and Capture simulation via runSimulation") {
//    Game game;
//
//    // הגדרת הקלט בדיוק בפורמט שהפונקציה מצפה לו
//    std::vector<std::string> inputLines = {
//        " Board:",
//        ". . .",
//        "wK bR .",
//        ". . .",
//        "Commands:",
//        "jump 50 150",   // מבצע קפיצה על ה-wK ב-(1,0)
//        "click 150 150", // בוחר ב-bR ב-(1,1)
//        "click 50 150",  // מנסה לזוז למיקום של ה-wK
//        "wait 1000"      // ממתין לסיום התנועה והקפיצה
//    };
//
//    // הרצת הסימולציה המלאה
//    game.runSimulation(inputLines);
//
//    // בדיקה לאחר סיום הסימולציה
//    const Board& board = game.getBoard();
//
//    // ה-wK היה ב-(1,0) ונשאר שם (הקפיצה לא מזיזה אותו)
//    CHECK(board.getPieceType(Position(1, 0)) == PieceType::King);
//
//    // ה-bR שהגיע ל-(1,1) היה אמור להיאכל על ידי ה-airborne wK
//    // לכן המשבצת (1,1) צריכה להיות ריקה
//    CHECK(board.isEmpty(Position(1, 1)) == true);
//
//    // נוודא שהלוח נראה כפי שצפוי
//    // הלוח אמור להיראות כך:
//    // . . .
//    // wK . .
//    // . . .
//}
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
//TEST_CASE("Test 47: airborne_piece_captures_arriving_enemy") {
//    Game game;
//    std::vector<std::string> boardSetup = {
//        ". . .",
//        "wK bR .",
//        ". . ."
//    };
//    game.setupBoard(boardSetup);
//
//    // ביצוע הפקודות
//    // הערה: נראה שיש לך פקודת "jump" בטסט, וודא שיש לך מימוש לזה ב-Game
//    // game.executeJump(50, 150); 
//    game.executeClick(150, 150);
//    game.executeClick(50, 150);
//    game.executeWait(1000);
//
//    const Board& b = game.getBoard();
//    // בדיקה שהמלך הלבן נשאר במיקומו המקורי (או עבר ליעד)
//    CHECK(b.getPieceType(Position(1, 0)) == PieceType::King);
//}

//TEST_CASE("Test 7: promoted_queen_moves_diagonal - Debugging") {
//    Game game;
//    // הגדרת הלוח לפי הטסט
//    std::vector<std::string> boardSetup = {
//        ". . .",
//        ". wP .",
//        ". . ."
//    };
//    game.setupBoard(boardSetup);
//
//    // מהלך 1: חייל לוקח צעד (קידום למלכה)
//    // click 150 150 -> click 150 50
//    game.executeClick(150, 150);
//    game.executeClick(150, 50);
//    game.executeWait(1000);
//
//    // בדיקה שהקידום קרה (הכלי ב-(0,1) צריך להיות מלכה)
//    const Board& board = game.getBoard();
//    CHECK(board.getPieceType(Position(0, 1)) == PieceType::Queen);
//
//    // מהלך 2: תנועת המלכה באלכסון
//    // click 150 50 -> click 250 150
//    game.executeClick(150, 50);
//    game.executeClick(250, 150);
//    game.executeWait(1000);
//
//    // בדיקה שהמלכה אכן זזה ל-(1, 2)
//    CHECK(board.isEmpty(Position(0, 1)) == true);
//    CHECK(board.getPieceType(Position(1, 2)) == PieceType::Queen);
//    CHECK(board.getPieceColor(Position(1, 2)) == PieceColor::White);
//}
//TEST_CASE("Testing Board parsing and simulation execution") {
//    Game game;
//
//    std::vector<std::string> inputLines = {
//        " Board:",
//        "wR . .",
//        ". . .",
//        "bR . .",
//        "Commands:",
//        "click 50 50",
//        "click 250 50",
//        "click 50 250",
//        "click 250 250",
//        "wait 2000",
//        "print board"
//    };
//
//    // הרצת הסימולציה
//    game.runSimulation(inputLines);
//
//    // בדיקה שהלוח לא ריק (אופציונלי)
//    const Board& b = game.getBoard();
//    CHECK(b.getRows() == 3);
//    CHECK(b.getCols() == 3);
//}

//#include "doctest.h"
//#include "Game.h"
//
//TEST_SUITE("Game Simulations")
//{
//    TEST_CASE("Standard simulation match format")
//    {
//        Game game;
//
//        game.runSimulation({
//            "Board:",
//            "wR . .",
//            ". . .",
//            "bR . .",
//            "Commands:",
//            "click 50 50",
//            "click 250 50",
//            "click 50 250",
//            "click 250 250",
//            "wait 2000",
//            "print board"
//            });
//
//        CHECK(game.getBoard().isEmpty(Position(0, 0)));
//        CHECK(game.getBoard().getPieceType(Position(0, 2)) == PieceType::Rook);
//    }
//
//    TEST_CASE("Another simulation test case")
//    {
//        Game game;
//
//        game.runSimulation({
//            "Board:",
//            "wK . .",
//            ". . .",
//            "Commands:",
//            "click 50 50",
//            "click 150 50",
//            "wait 1000"
//            });
//
//        CHECK(game.getBoard().getPieceType(Position(0, 1)) == PieceType::King);
//    }
//}