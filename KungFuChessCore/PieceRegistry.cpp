#include "pch.h"
#include "PieceRegistry.h"

const PieceMetadata& PieceRegistry::getMetadata(PieceType type) {
    static const std::map<PieceType, PieceMetadata> data = {
        {PieceType::Pawn,   {"Pawn",   "wP", "bP", "assets/pawn.png"}},
        {PieceType::Rook,   {"Rook",   "wR", "bR", "assets/rook.png"}},
        {PieceType::Knight, {"Knight", "wN", "bN", "assets/knight.png"}},
        {PieceType::Bishop, {"Bishop", "wB", "bB", "assets/bishop.png"}},
        {PieceType::Queen,  {"Queen",  "wQ", "bQ", "assets/queen.png"}},
        {PieceType::King,   {"King",   "wK", "bK", "assets/king.png"}},
    };

    auto it = data.find(type);
    if (it != data.end()) {
        return it->second;
    }
}
