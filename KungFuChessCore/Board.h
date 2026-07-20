#pragma once
#include "Piece.h"
#include "PieceTypes.h"
#include "Position.h"
#include <memory>
#include <string>
#include <vector>
using namespace std;

class IBoardPrinter;

class Board
{
    friend class TextBoardConvert;

public:
    Board();

    void clear();
    void setSize(int rows, int cols);
    void setPiece(Position place, std::unique_ptr<Piece> piece);
    Piece* getPiece(Position place) const;
    void movePiece(Position from, Position to);
    void removePiece(Position pos);
    void promotePiece(Position pos, PieceType newType);
    void print(IBoardPrinter& printer) const;

    bool isInside(Position pos) const;
    bool isEmpty(Position pos) const;
    bool isPieceAt(int row, int col, PieceType type) const;
    PieceColor getPieceColor(Position pos) const;
    PieceType getPieceType(Position pos) const;

    int getRows() const;
    int getCols() const;

    void setError(const string& error);
    string getError() const;

private:
    vector<vector<unique_ptr<Piece>>> cells;
    string errorMessage;
};
