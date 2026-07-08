#include "pch.h"
#include "Board.h"
#include <iostream>
#include <sstream>
using namespace std;

Board::Board()
{
    rows = 0;
    cols = 0;
    errorMessage = "";
}

bool Board::isValidToken(const string& token) const
{
    if (token == ".") return true;
    if (token.size() != 2) return false;

    char color = token[0];
    char piece = token[1];

    if (color != 'w' && color != 'b') return false;
    if (piece != 'K' && piece != 'Q' && piece != 'R' && piece != 'B' && piece != 'N' && piece != 'P') return false;

    return true;
}

void Board::parse(const vector<string>& input)
{
    cells.clear();
    errorMessage = "";
    rows = 0;
    cols = -1;

    for (const auto& line : input)
    {
        vector<string> rowTokens;
        stringstream ss(line);
        string token;

        while (ss >> token)
        {
            rowTokens.push_back(token);
        }

        if (rowTokens.empty()) continue;

        for (const auto& t : rowTokens)
        {
            if (!isValidToken(t))
            {
                if (errorMessage.empty()) {
                    errorMessage = "ERROR UNKNOWN_TOKEN";
                }
            }
        }

        if (cols == -1)
        {
            cols = rowTokens.size();
        }
        else if ((int)rowTokens.size() != cols)
        {
            if (errorMessage.empty()) {
                errorMessage = "ERROR ROW_WIDTH_MISMATCH";
            }
        }

        cells.push_back(rowTokens);
    }

    rows = cells.size();
    if (rows == 0) cols = 0;
}

bool Board::validate() const
{
    return errorMessage.empty();
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

int Board::getRows() const { return rows; }
int Board::getCols() const { return cols; }
string Board::getError() const { return errorMessage; }

string Board::getTile(int r, int c) const {
    return cells[r][c];
}

void Board::setTile(int r, int c, const std::string& token) {
    cells[r][c] = token;
}