#pragma once
#include <vector>
#include <string>

class GameSnapshot;

class IBoardPrinter
{
public:
    virtual ~IBoardPrinter() = default;

    virtual void Convert(const GameSnapshot& snapshot) = 0;
};
