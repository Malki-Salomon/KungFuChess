#include "pch.h"
#include "Board.h"
#include "IBoardPrinter.h"
#include "PieceFactory.h"
#include "PieceTypes.h"
#include "Snapshot.h"
using namespace std;

Board::Board()
{
    errorMessage = "";
}

void Board::clear()
{
    cells.clear();
    errorMessage.clear();
}

void Board::setSize(int row, int col)
{
    cells.resize(row);
    for (auto& r : cells)
    {
        r.resize(col);
    }
}

void Board::setPiece(Position place, std::unique_ptr<Piece> piece)
{
    cells[place.getRow()][place.getCol()] = std::move(piece);
}

Piece* Board::getPiece(Position place) const
{
    return cells[place.getRow()][place.getCol()].get();
}

void Board::movePiece(Position from, Position to)
{
    setPiece(to, std::move(cells[from.getRow()][from.getCol()]));
}

void Board::removePiece(Position pos)
{
    setPiece(pos, nullptr);
}

void Board::promotePiece(Position pos, PieceType newType)
{
    PieceColor color = getPieceColor(pos);
    cells[pos.getRow()][pos.getCol()] = PieceFactory::create(newType, color, pos);
}

void Board::print(IBoardPrinter& printer) const
{
    if (!errorMessage.empty())
    {
        return;
    }
    printer.Convert(Snapshot::build(*this));
}

bool Board::isInside(Position pos) const
{
    return pos.getRow() >= 0 &&
        pos.getRow() < cells.size() &&
        pos.getCol() >= 0 &&
        pos.getCol() < cells[0].size();
}

bool Board::isEmpty(Position pos) const
{
    if (!isInside(pos))
        return false;
    return cells[pos.getRow()][pos.getCol()] == nullptr;
}

bool Board::isPieceAt(int row, int col, PieceType type) const
{
    return cells[row][col] != nullptr && cells[row][col]->getType() == type;
}

PieceColor Board::getPieceColor(Position pos) const
{
    return cells[pos.getRow()][pos.getCol()]->getColor();
}

PieceType Board::getPieceType(Position pos) const
{
    return cells[pos.getRow()][pos.getCol()]->getType();
}

int Board::getRows() const
{
    return cells.size();
}

int Board::getCols() const
{
    if (cells.empty())
        return 0;
    return cells[0].size();
}

void Board::setError(const string& error)
{
    errorMessage = error;
}

string Board::getError() const
{
    return errorMessage;
}
