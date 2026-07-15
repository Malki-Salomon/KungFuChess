#include "pch.h"
#include "Bishop.h"
#include "Board.h"
#include "King.h"
#include "Knight.h"
#include "Pawn.h"
#include "PieceTypes.h"
#include "Queen.h"
#include "Rook.h"
#include <iostream>
#include <sstream>
#include "PieceFactory.h"
#include "IBoardPrinter.h"
using namespace std;

Board::Board()
{
    errorMessage = "";
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

void Board::print(IBoardPrinter& printer) const
{
    if (!errorMessage.empty()) { // הוסף את הבדיקה הזו
        //cout << errorMessage << endl;
        return;
    }
	printer.Convert(*this);
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
    return cells[pos.getRow()][pos.getCol()]== nullptr;
}

PieceColor Board::getPieceColor(Position pos) const
{
    return cells[pos.getRow()][pos.getCol()]->getColor();
}

PieceType Board::getPieceType(Position pos) const
{
	return cells[pos.getRow()][pos.getCol()]->getType();
}

void Board::movePiece(Position from, Position to)
{
	setPiece(to, std::move(cells[from.getRow()][from.getCol()]));
}

void Board::removePiece(Position pos)
{
    setPiece(pos, nullptr);
}

bool Board::isPieceAt(int row, int col, PieceType type) const
{
	return cells[row][col] != nullptr && cells[row][col]->getType() == type;
}

void Board::promotePiece(Position pos, PieceType newType) {
    PieceColor color = getPieceColor(pos);
    cells[pos.getRow()][pos.getCol()] = PieceFactory::create(newType, color, pos);
}

//bool Board::validate() const
//{
//    return errorMessage.empty();
//}
//
void Board::setError(const string& error)
{
    errorMessage = error;
}

string Board::getError() const { return errorMessage; }

//bool Board::isValidToken(const string& token) const
//{
//    if (token == ".") return true;
//    if (token.size() != 2) return false;
//
//    char color = token[0];
//    char piece = token[1];
//
//    if (color != 'w' && color != 'b') return false;
//    if (piece != 'K' && piece != 'Q' && piece != 'R' && piece != 'B' && piece != 'N' && piece != 'P') return false;
//
//    return true;
//}

//void Board::parse(const vector<string>& input)
//{
//    cells.clear();
//    errorMessage = "";
//    rows = 0;
//    cols = -1;
//
//    for (const auto& line : input)
//    {
//        vector<string> rowTokens;
//        stringstream ss(line);
//        string token;
//
//        while (ss >> token)
//        {
//            rowTokens.push_back(token);
//        }
//
//        if (rowTokens.empty()) continue;
//
//        for (const auto& t : rowTokens)
//        {
//            if (!isValidToken(t))
//            {
//                if (errorMessage.empty()) {
//                    errorMessage = "ERROR UNKNOWN_TOKEN";
//                }
//            }
//        }
//
//        if (cols == -1)
//        {
//            cols = rowTokens.size();
//        }
//        else if ((int)rowTokens.size() != cols)
//        {
//            if (errorMessage.empty()) {
//                errorMessage = "ERROR ROW_WIDTH_MISMATCH";
//            }
//        }
//
//        cells.push_back(rowTokens);
//    }
//
//    rows = cells.size();
//    if (rows == 0) cols = 0;
//}

//void Board::initialize(
//    const vector<vector<Piece*>>& newCells,
//    const string& error)
//{
//    cells = newCells;
//    errorMessage = error;
//
//    rows = cells.size();
//
//    if (rows == 0)
//        cols = 0;
//    else
//        cols = cells[0].size();
//}

//void Board::setEmpty(int row, int col)
//{
//    cells[row][col] = ".";
//}
