#include "pch.h"
#include "Bishop.h"
#include "Game.h"
#include "King.h"
#include "Knight.h"
#include "Queen.h"
#include "Rook.h"
#include <iostream>
#include <memory>
using namespace std;

Game::Game() {
    gameClockMs = 0;
    hasSelection = false;
    selectedRow = -1;
    selectedCol = -1;
}

void Game::setupBoard(const vector<string>& boardLines) {
    board.parse(boardLines);
}

bool Game::isBoardValid() const {
    return board.validate();
}

string Game::getBoardError() const {
    return board.getError();
}

void Game::executeClick(int x, int y)
{
    if (x < 0 || y < 0)
        return;

    int c = x / 100;
    int r = y / 100;

    if (r < 0 || r >= board.getRows() ||
        c < 0 || c >= board.getCols())
        return;

    std::string clickedToken = board.getTile(r, c);

    // אין כלי מסומן - בוחרים כלי
    if (!hasSelection)
    {
        if (clickedToken != ".")
        {
            hasSelection = true;
            selectedRow = r;
            selectedCol = c;
        }
        return;
    }

    std::string selectedToken = board.getTile(selectedRow, selectedCol);

    // לחיצה על כלי מאותו צבע - מחליפים בחירה
    if (clickedToken != "." &&
        clickedToken[0] == selectedToken[0])
    {
        selectedRow = r;
        selectedCol = c;
        return;
    }

    std::unique_ptr<Piece> piece;

    switch (selectedToken[1])
    {
    case 'K':
        piece = std::make_unique<King>(selectedToken[0]);
        break;

    case 'Q':
        piece = std::make_unique<Queen>(selectedToken[0]);
        break;

    case 'R':
        piece = std::make_unique<Rook>(selectedToken[0]);
        break;

    case 'B':
        piece = std::make_unique<Bishop>(selectedToken[0]);
        break;

    case 'N':
        piece = std::make_unique<Knight>(selectedToken[0]);
        break;

    default:
        hasSelection = false;
        return;
    }

    // בדיקת חוקיות המהלך
    if (!piece->isValidMove(selectedRow,
        selectedCol,
        r,
        c,
        board))
    {
        return;
    }

    // ביצוע המהלך
    board.setTile(r, c, selectedToken);
    board.setTile(selectedRow, selectedCol, ".");

    hasSelection = false;
}

void Game::executeWait(long long ms) {
    if (ms > 0) {
        gameClockMs += ms;
    }
}

void Game::printBoard() const {
    board.print();
}