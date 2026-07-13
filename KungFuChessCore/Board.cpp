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
using namespace std;

Board::Board()
{
    /*errorMessage = "";*/
}

void Board::clear()
{
    cells.clear();

    //errorMessage.clear();
}

void Board::setSize(int row, int col)
{
    cells.resize(row);
    for (auto& r : cells)
    {
        r.resize(col);
    }
}

void Board::setPiece(int row, int col, std::unique_ptr<Piece> piece)
{
    cells[row][col] = std::move(piece);
}

unique_ptr<Piece>& Board::getPiece(int row, int col)  
{  
    return cells[row][col];  
}

void Board::print() const
{
    for (size_t i = 0; i < cells.size(); ++i)
    {
        for (size_t j = 0; j < cells[i].size(); ++j)
        {
            cout << cells[i][j];
            if (j < cells[i].size() - 1) {
                cout << " ";
            }
        }
        cout << endl;
    }
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

void Board::movePiece(int fromRow, int fromCol, int toRow,  int toCol)
{
	setPiece(toRow, toCol, std::move(cells[fromRow][fromCol]));
	setPiece(fromRow, fromCol, nullptr);
}

//bool Board::validate() const
//{
//    return errorMessage.empty();
//}
//
//void Board::setError(const string& error)
//{
//    errorMessage = error;
//}
//
//string Board::getError() const { return errorMessage; }

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
