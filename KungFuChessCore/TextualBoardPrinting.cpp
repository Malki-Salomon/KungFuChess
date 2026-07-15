#include "pch.h"
#include "TextualBoardPrinting.h"
#include "Board.h"
#include <iostream>

void TextualBoardPrinting::Convert(const Board& board)
{
    for (size_t i = 0; i < board.getRows(); ++i)
    {
        for (size_t j = 0; j < board.getCols(); ++j)
        {
            if (!board.isEmpty(Position(i, j))) {
                cout << board.getPiece(Position(i, j))->getName();
            }
            else {
                cout << ".";
            }
            if (j < board.getCols() - 1) {
                cout << " ";
            }
        }
        cout << endl;
    }
}
