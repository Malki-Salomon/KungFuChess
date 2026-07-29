#include "pch.h"
#include "Game.h"
#include "IBoardPrinter.h"
#include "RuleEngine.h"
#include "Snapshot.h"
#include "TextualBoardPrinting.h"
#include <iostream>

using namespace std;

Game::Game()
{
    hasSelection = false;
    selectedPiece = Position(-1, -1);
    gameStatus = GameStatus::Playing;
	printer = nullptr;
    boardReady = false;
}

void Game::setupBoard(IBoardConvert& converter)
{
    if (!converter.Convert(board))
    {
        cout << board.getError() << endl;
        return;
    }
    boardReady = true;
    publishSnapshotIfReady();
}

void Game::addCommand(Command cmd) {
    commandQueue.push(cmd);
}

bool Game::hasCommands() const {
    return !commandQueue.empty();
}

void Game::processNextCommand()
{
    if (commandQueue.empty())
    {
        return;
    }
	auto& cmd = commandQueue.front();
	commandQueue.pop();
    switch (cmd.type)
    {
    case CommandType::Click:
        controller.executeClick(cmd.x, cmd.y, *this, this->arbiter);
        break;
    case CommandType::Jump:
        controller.executeJump(cmd.x, cmd.y, *this, this->arbiter);
        break;
    case CommandType::Wait:
    {
        RuleEngine ruleEngine;
        bool someChange = this->arbiter.tick(cmd.ms, this->board, ruleEngine, *this);
		if (someChange && this->printer)
			board.print(*this->printer);
        break;
    }
    case CommandType::Print:
        if (this->printer)
            board.print(*this->printer);
        break;
    }
}

void Game::gameOver(Board& board, Position place)
{
    if (board.getPiece(place) && board.getPieceType(place) == PieceType::King)
    {
        board.getPieceColor(place) == PieceColor::White ? gameStatus = GameStatus::BlackWins : gameStatus = GameStatus::WhiteWins;
    }
}

bool Game::isGameActive() const
{
    return gameStatus == GameStatus::Playing;
}

GameStatus Game::getStatus() const
{
    return gameStatus;
}

const Board& Game::getBoard() const
{
    return board;
}

GameSnapshot Game::getSnapshot() const
{
    return Snapshot::build(board);
}

bool Game::gethasSelection() const
{
    return hasSelection;
}

Position Game::getSelectedPiece() const
{
    return selectedPiece;
}

void Game::setHasSelection(bool value)
{
    hasSelection = value;
}

void Game::setSelectedPiece(Position pos)
{
    selectedPiece = pos;
}

void Game::setPrinter(IBoardPrinter* printer)
{
    this->printer = printer;
    publishSnapshotIfReady();
}

void Game::publishSnapshotIfReady()
{
    if (boardReady && printer != nullptr)
    {
        board.print(*printer);
    }
}
