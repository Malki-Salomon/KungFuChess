#include "pch.h"
#include "TextualBoardPrinting.h"
#include "GameSnapshot.h"
#include <iostream>
#include "PieceRegistry.h"

void TextualBoardPrinting::Convert(const GameSnapshot& snapshot)
{
    for (const auto& row : snapshot.cells)
    {
        for (const auto& cell : row)
        {
            if (cell.type != PieceType::Empty) {
                const auto& meta = PieceRegistry::getMetadata(cell.type);

                if (cell.color == PieceColor::White) {
                    std::cout << meta.whiteSymbol; 
                }
                else {
                    std::cout << meta.blackSymbol; 
                }
            }
            else {
                std::cout << ".";
            }

            std::cout << " ";
        }
        std::cout << std::endl;
    }
}
