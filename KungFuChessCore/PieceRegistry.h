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
    // פונקציה סטטית להחזרת נתוני הכלי
    static const PieceMetadata& getMetadata(PieceType type);
};