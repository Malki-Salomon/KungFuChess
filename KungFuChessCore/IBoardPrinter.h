#pragma once
#include <vector>
#include <string>

class Board;

class IBoardPrinter
{
public:
    virtual ~IBoardPrinter() = default;

    virtual void Convert(const Board& board) = 0;
};
