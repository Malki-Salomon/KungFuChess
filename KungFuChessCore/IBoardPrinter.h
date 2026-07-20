#pragma once
#include "GameSnapshot.h"
#include <vector>
#include <string>

class IBoardPrinter
{
public:
    virtual ~IBoardPrinter() = default;

    virtual void Convert(const GameSnapshot& snapshot) = 0;
};
