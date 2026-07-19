#pragma once
#include "IBoardPrinter.h"
class TextualBoardPrinting : public IBoardPrinter
{
public:
	void Convert(const GameSnapshot& snapshot) override;
};

