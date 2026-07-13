#pragma once

#include "IBoardConvert.h"
#include <vector>
#include <string>

class TextBoardConvert : public IBoardConvert
{
private:
    std::vector<std::string> lines;

    bool isValidToken(const std::string& token) const;

public:
    TextBoardConvert(const std::vector<std::string>& input);

    bool Convert(Board& board) override;
};