#pragma once
#include <vector>
#include <string>

class Board;

class IBoardConvert
{
public:
    virtual ~IBoardConvert() = default;

    virtual bool Convert(Board& board) = 0;
};

