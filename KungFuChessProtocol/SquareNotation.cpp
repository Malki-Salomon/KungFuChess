#include "SquareNotation.h"

#include <cctype>

namespace Protocol
{
    std::string SquareNotation::toNotation(int row, int col)
    {
        if (row < 0 || row > 7 || col < 0 || col > 7)
            return std::string();

        char file = static_cast<char>('a' + col);
        char rank = static_cast<char>('0' + (8 - row));

        return std::string(1, file) + std::string(1, rank);
    }

    bool SquareNotation::tryParse(const std::string& notation, Square& square)
    {
        if (notation.size() != 2)
            return false;

        char file = static_cast<char>(std::tolower(static_cast<unsigned char>(notation[0])));
        char rank = notation[1];

        if (file < 'a' || file > 'h')
            return false;
        if (rank < '1' || rank > '8')
            return false;

        square.col = file - 'a';
        square.row = 8 - (rank - '0');
        return true;
    }
}
