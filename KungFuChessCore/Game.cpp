#include "pch.h"
#include "Bishop.h"
#include "Game.h"
#include "King.h"
#include "Knight.h"
#include "Queen.h"
#include "Rook.h"
#include <iostream>
#include <memory>
#include <Pawn.h>
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

    bool clickedEmpty = board.isEmpty(r, c);

    // No tool selected - select tool
    if (!hasSelection)
    {
        if (!board.isEmpty(r, c))
        {
            hasSelection = true;
            selectedRow = r;
            selectedCol = c;
        }
        return;
    }

    PieceType selectedType =
        board.getPieceType(selectedRow, selectedCol);

    PieceColor selectedColor =
        board.getPieceColor(selectedRow, selectedCol);

    // Clicking on a tool of the same color - replaces selection
    if (!board.isEmpty(r, c) &&
        board.getPieceColor(r, c) ==
        board.getPieceColor(selectedRow, selectedCol))
    {
        selectedRow = r;
        selectedCol = c;
        return;
    }

    std::unique_ptr<Piece> piece;

    switch (selectedType)
    {
    case PieceType::King:
        piece = std::make_unique<King>(selectedColor);
        break;

    case PieceType::Queen:
        piece = std::make_unique<Queen>(selectedColor);
        break;

    case PieceType::Rook:
        piece = std::make_unique<Rook>(selectedColor);
        break;

    case PieceType::Bishop:
        piece = std::make_unique<Bishop>(selectedColor);
        break;

    case PieceType::Knight:
        piece = std::make_unique<Knight>(selectedColor);
        break;

    case PieceType::Pawn:
        piece = std::make_unique<Pawn>(selectedColor);
        break;

    default:
        hasSelection = false;
        return;
    }

    // Checking the legality of the move
    if (!piece->isValidMove(selectedRow,
        selectedCol,
        r,
        c,
        board))
    {
        return;
    }

    // Making the move
    board.movePiece(selectedRow,
        selectedCol,
        r,
        c);

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