#pragma once
#include <vector>
#include "PieceTypes.h"

struct PieceSnapshot {
    PieceType type;
    PieceColor color;
};

struct GameSnapshot {
    std::vector<std::vector<PieceSnapshot>> cells;
};