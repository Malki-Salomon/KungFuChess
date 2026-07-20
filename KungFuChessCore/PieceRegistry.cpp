#include "pch.h"
#include "PieceRegistry.h"

const PieceMetadata& PieceRegistry::getMetadata(PieceType type) {
    static const std::map<PieceType, PieceMetadata> data = {
        {PieceType::Pawn,   {"Pawn",   "wP", "bP", ".\\pictures\\PW\\states\\idle\\sprites\\1.png", ".\\pictures\\PB\\states\\idle\\sprites\\1.png"}},
        {PieceType::Rook,   {"Rook",   "wR", "bR", ".\\pictures\\RW\\states\\idle\\sprites\\1.png", ".\\pictures\\RB\\states\\idle\\sprites\\1.png"}},
        {PieceType::Knight, {"Knight", "wN", "bN", ".\\pictures\\NW\\states\\idle\\sprites\\1.png", ".\\pictures\\NB\\states\\idle\\sprites\\1.png"}},
        {PieceType::Bishop, {"Bishop", "wB", "bB", ".\\pictures\\BW\\states\\idle\\sprites\\1.png", ".\\pictures\\BB\\states\\idle\\sprites\\1.png"}},
        {PieceType::Queen,  {"Queen",  "wQ", "bQ", ".\\pictures\\QW\\states\\idle\\sprites\\1.png", ".\\pictures\\QB\\states\\idle\\sprites\\1.png"}},
        {PieceType::King,   {"King",   "wK", "bK", ".\\pictures\\KW\\states\\idle\\sprites\\1.png", ".\\pictures\\KB\\states\\idle\\sprites\\1.png"}},
    };

    auto it = data.find(type);
    if (it != data.end()) {
        return it->second;
    }
}
