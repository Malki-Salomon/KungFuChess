#pragma once
#include "PieceTypes.h"
#include <vector>
#include <string>
using namespace std;

class Board
{
private:
    int rows;
    int cols;
    vector<vector<string>> cells;
    string errorMessage;

    bool isValidToken(const string& token) const;

public:
    Board();

    void parse(const vector<string>& input);
    bool validate() const;
    void print() const;

    int getRows() const;
    int getCols() const;
    string getError() const;

    bool isEmpty(int row, int col) const;

    PieceType getPieceType(int row, int col) const;

    PieceColor getPieceColor(int row, int col) const;

    void movePiece(int fromRow, int fromCol,
        int toRow, int toCol);

    void setEmpty(int row, int col);




    string getTile(int r, int c) const;
    void setTile(int r, int c, const string& token);
};

