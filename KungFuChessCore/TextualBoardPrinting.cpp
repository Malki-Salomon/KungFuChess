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
                // קבלת המידע מה-Registry
                const auto& meta = PieceRegistry::getMetadata(cell.type);

                // בחירת הסמל לפי צבע הכלי מה-Snapshot
                if (cell.color == PieceColor::White) {
                    std::cout << meta.whiteSymbol; // בהנחה שזה השדה wP
                }
                else {
                    std::cout << meta.blackSymbol; // בהנחה שזה השדה bP
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
