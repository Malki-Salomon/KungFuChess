#include "pch.h"
#include "Bishop.h"
#include "Game.h"
#include "King.h"
#include "Knight.h"
#include "Pawn.h"
#include "Queen.h"
#include "Rook.h"
#include "TextBoardConvert.h"
#include <iostream>
#include <memory>
#include "RuleEngine.h"
#include "StringCommandConvert.h"

using namespace std;

Game::Game() {
    gameClockMs = 0;
    hasSelection = false;
    selectedRow = -1;
    selectedCol = -1;
    moveInProgress = false;
    remainingMoveTime = 0;
}

void Game::setupBoard(const vector<string>& boardLines) {
    /*board.parse(boardLines);*/
    TextBoardConvert converter(boardLines);
    converter.Convert(board);
}

//bool Game::isBoardValid() const {
//    return board.validate();
//}

//string Game::getBoardError() const {
//    return board.getError();
//}

//void Game::executeClick(int x, int y)
//{
//    if (moveInProgress)
//        return;
//
//    if (x < 0 || y < 0)
//        return;
//
//    int c = x / 100;
//    int r = y / 100;
//
//    if (r < 0 || r >= board.getRows() ||
//        c < 0 || c >= board.getCols())
//        return;
//
//    bool clickedEmpty = board.isEmpty(r, c);
//
//    // No tool selected - select tool
//    if (!hasSelection)
//    {
//        if (!board.isEmpty(r, c))
//        {
//            hasSelection = true;
//            selectedRow = r;
//            selectedCol = c;
//        }
//        return;
//    }
//
//    PieceType selectedType =
//        board.getPieceType(selectedRow, selectedCol);
//
//    PieceColor selectedColor =
//        board.getPieceColor(selectedRow, selectedCol);
//
//    // Clicking on a tool of the same color - replaces selection
//    if (!board.isEmpty(r, c) &&
//        board.getPieceColor(r, c) ==
//        board.getPieceColor(selectedRow, selectedCol))
//    {
//        selectedRow = r;
//        selectedCol = c;
//        return;
//    }
//
//    std::unique_ptr<Piece> piece;
//
//    switch (selectedType)
//    {
//    case PieceType::King:
//        piece = std::make_unique<King>(selectedColor);
//        break;
//
//    case PieceType::Queen:
//        piece = std::make_unique<Queen>(selectedColor);
//        break;
//
//    case PieceType::Rook:
//        piece = std::make_unique<Rook>(selectedColor);
//        break;
//
//    case PieceType::Bishop:
//        piece = std::make_unique<Bishop>(selectedColor);
//        break;
//
//    case PieceType::Knight:
//        piece = std::make_unique<Knight>(selectedColor);
//        break;
//
//    case PieceType::Pawn:
//        piece = std::make_unique<Pawn>(selectedColor);
//        break;
//
//    default:
//        hasSelection = false;
//        return;
//    }
//
//    // Checking the legality of the move
//    /*if (!piece->isValidMove(selectedRow,
//        selectedCol,
//        r,
//        c,
//        board))
//    {
//        return;
//    }*/
//    bool valid = piece->isValidMove(
//        selectedRow,
//        selectedCol,
//        r,
//        c,
//        board);
//
//    if (!valid)
//    {
//        return;
//    }
//
//    moveInProgress = true;
//
//    moveFromRow = selectedRow;
//    moveFromCol = selectedCol;
//
//    moveToRow = r;
//    moveToCol = c;
//
//    remainingMoveTime =
//        std::max(abs(moveToRow - moveFromRow),
//            abs(moveToCol - moveFromCol)) * 1000;
//
//
//    hasSelection = false;
//}

void Game::runSimulation(const std::vector<std::string>& inputLines) {
    std::vector<std::string> boardLines;
    std::vector<std::string> commandLines;
    bool parsingCommands = false;

    for (const auto& line : inputLines) {
        if (line.empty()) continue;

        if (line == "Commands:") {
            parsingCommands = true;
            continue;
        }
        if (line == "Board:") {
            continue;
        }

        if (!parsingCommands) {
            boardLines.push_back(line);
        }
        else {
            commandLines.push_back(line);
        }
    }

    // 1. טעינת הלוח
    if (!boardLines.empty()) {
        setupBoard(boardLines);
    }

    // 2. הרצת הפקודות בעזרת התשתית הקיימת שלך!
    StringCommandConvert commandConverter(commandLines);
    auto commands = commandConverter.Convert();

    for (const auto& cmd : commands) {
        if (cmd) {
            cmd->execute(*this);
        }
    }
}

void Game::executeClick(int x, int y)
{
    if (moveInProgress)
        return;

    if (x < 0 || y < 0)
        return;

    int c = x / 100;
    int r = y / 100;

    if (!board.isInside(Position(r, c)))
        return;

    // אין כלי מסומן - בוחרים כלי
    if (!hasSelection)
    {
        if (!board.isEmpty(Position(r, c)))
        {
            hasSelection = true;
            selectedRow = r;
            selectedCol = c;
        }
        return;
    }

    // לחיצה על כלי מאותו צבע - מחליפים בחירה
    if (!board.isEmpty(Position(r, c)) &&
        board.getPieceColor(Position(r, c)) ==
        board.getPieceColor(Position(selectedRow, selectedCol)))
    {
        selectedRow = r;
        selectedCol = c;
        return;
    }

    // בדיקת חוקיות
    if (!RuleEngine::isLegalMove(board, Position(selectedRow, selectedCol), Position(r, c)))
    {
        return;
    }

    // התחלת המהלך
    moveInProgress = true;

    moveFromRow = selectedRow;
    moveFromCol = selectedCol;

    moveToRow = r;
    moveToCol = c;

    remainingMoveTime =
        std::max(abs(moveToRow - moveFromRow),
            abs(moveToCol - moveFromCol)) * 1000;

    hasSelection = false;
}

void Game::executeWait(long long ms)
{
    if (ms <= 0)
        return;

    gameClockMs += ms;

    if (!moveInProgress)
        return;

    remainingMoveTime -= ms;

    if (remainingMoveTime <= 0)
    {
        board.movePiece(
            moveFromRow,
            moveFromCol,
            moveToRow,
            moveToCol);

        moveInProgress = false;
    }
}

void Game::printBoard() const {
    board.print();
}

const Board& Game::getBoard() const
{
    return board;
}