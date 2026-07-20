#pragma once
#include <string>
#include <map>
#include "PieceTypes.h"

struct PieceMetadata {
    std::string name;
    std::string whiteSymbol;
    std::string blackSymbol;
    std::string whiteImagePath;
    std::string blackImagePath;
};

class PieceRegistry {
public:
    static const PieceMetadata& getMetadata(PieceType type);
};