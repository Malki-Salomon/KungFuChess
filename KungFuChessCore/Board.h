#pragma once
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

    string getTile(int r, int c) const;
    void setTile(int r, int c, const string& token);
};

