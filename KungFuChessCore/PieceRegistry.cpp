#include "pch.h"
#include "PieceRegistry.h"

#include <utility>

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

    // PieceType::Empty (and any value not in the table above) has no
    // metadata to report - it isn't a piece. Previously this function just
    // ran off the end here, which is undefined behavior; this gives it one
    // defined answer instead. Blank rather than a fabricated symbol so a
    // caller that skipped its "is this square empty?" check gets visibly
    // nothing, instead of something that renders or serializes as if a
    // real piece were there.
    //
    // Deliberately NOT an exception: every caller already checks for Empty
    // first (see the header), and the server's room loop has no handler -
    // turning a caller's oversight into a process-wide crash would be a
    // worse failure mode than returning blanks.
    static const PieceMetadata noPiece{};
    return noPiece;
}

bool PieceRegistry::tryParseSymbol(const std::string& symbol, PieceType& outType, PieceColor& outColor) {
    static const std::map<std::string, std::pair<PieceType, PieceColor>> reverse = [] {
        std::map<std::string, std::pair<PieceType, PieceColor>> result;
        for (PieceType type : { PieceType::King, PieceType::Queen, PieceType::Rook,
                                 PieceType::Bishop, PieceType::Knight, PieceType::Pawn }) {
            const PieceMetadata& meta = PieceRegistry::getMetadata(type);
            result[meta.whiteSymbol] = { type, PieceColor::White };
            result[meta.blackSymbol] = { type, PieceColor::Black };
        }
        return result;
    }();

    auto it = reverse.find(symbol);
    if (it == reverse.end()) {
        return false;
    }

    outType = it->second.first;
    outColor = it->second.second;
    return true;
}
