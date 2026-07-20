#pragma once
#include "IBoardPrinter.h"
#include "img.hpp"

class GuiBoardPrinting : public IBoardPrinter
{
public:
    void Convert(const GameSnapshot& snapshot) override;

private:
    Img boardImg;
};
