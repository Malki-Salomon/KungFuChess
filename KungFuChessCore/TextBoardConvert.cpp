#include "pch.h"
#include "TextBoardConvert.h"
#include "Board.h"

#include <sstream>
#include "PieceFactory.h"

using namespace std;

TextBoardConvert::TextBoardConvert(
    const vector<string>& input)
    : lines(input)
{
}

bool TextBoardConvert::isValidToken(
    const string& token) const
{
    if (token == ".")
        return true;

    if (token.size() != 2)
        return false;

    char color = token[0];
    char piece = token[1];

    if (color != 'w' && color != 'b')
        return false;

    switch (piece)
    {
    case 'K':
    case 'Q':
    case 'R':
    case 'B':
    case 'N':
    case 'P':
        return true;

    default:
        return false;
    }
}

bool TextBoardConvert::Convert(Board& board)
{
    board.clear();

    if (lines.empty())
        return true;

    int num_col = 0;
    {
        stringstream ss(lines[0]);
        string token;
        while (ss >> token)
            num_col++;
    }

    board.setSize(lines.size(), num_col);

    int row = 0;
    for (const auto& line : lines)
    {
        stringstream ss(line);
        string token;
        int col = 0;

        int current_row_cols = 0;
        stringstream count_ss(line);
        string temp;
        while (count_ss >> temp) current_row_cols++;

        if (current_row_cols != num_col)
        {
            board.setError("ERROR ROW_WIDTH_MISMATCH");
            return false;
        }

        while (ss >> token)
        {
            if (!isValidToken(token))
            {
                board.setError("ERROR UNKNOWN_TOKEN");
                return false;
            }

            if (token == ".")
            {
                board.setPiece(Position(row, col), nullptr);
            }
            else
            {
                PieceColor color = (token[0] == 'w') ? PieceColor::White : PieceColor::Black;
                PieceType type;

                switch (token[1])
                {
                case 'K': type = PieceType::King; break;
                case 'Q': type = PieceType::Queen; break;
                case 'R': type = PieceType::Rook; break;
                case 'B': type = PieceType::Bishop; break;
                case 'N': type = PieceType::Knight; break;
                case 'P': type = PieceType::Pawn; break;
                default:
                    board.setError("ERROR UNKNOWN_TOKEN");
                    return false;
                }

                board.setPiece(Position(row, col), PieceFactory::create(type, color, Position(row, col)));
            }
            col++;
        }
        row++;
    }
    return true;
}

