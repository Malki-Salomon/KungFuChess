#pragma once
#include "Piece.h"
#include "PieceTypes.h"
#include "Position.h"
#include <memory>
#include <vector>
#include <string>
using namespace std;

class TextBoardConvert;

class Board
{
    friend class TextBoardConvert;

private:
    vector<vector<unique_ptr<Piece>>> cells;
    //string errorMessage;

    //bool isValidToken(const string& token) const;

    /*void initialize(
        const std::vector<std::vector<unique_ptr<Piece>>>& newCells,
        const std::string& error);*/

public:
    Board();
    void clear();
	void setSize(int rows, int cols);
    void setPiece(int row, int col, std::unique_ptr<Piece> piece);
	unique_ptr<Piece>& getPiece(int row, int col);
    void print() const;
    bool isInside(Position pos) const;
    bool isEmpty(Position pos) const;
    PieceColor getPieceColor(Position pos) const;
    PieceType getPieceType(Position pos) const;
    void movePiece(int fromRow, int fromCol, int toRow, int toCol);
    

    /*bool validate() const;


	void setError(const string& error);
    string getError() const;*/






    



    //void parse(const vector<string>& input);
    //void setEmpty(int row, int col);

};


