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
    // Metadata for a real piece type. Callers are expected to have
    // established that the square actually holds a piece first - every
    // current caller checks `type != PieceType::Empty` before asking.
    //
    // For PieceType::Empty (or any value outside the six real pieces) there
    // is nothing to describe, and this returns an all-blank PieceMetadata
    // rather than being undefined. Treat a blank `name` as "not a piece":
    // it means the caller's own emptiness check was missed, not that a
    // piece exists with no symbol.
    static const PieceMetadata& getMetadata(PieceType type);

    // Reverse of getMetadata()'s type -> symbol mapping: given a board
    // symbol ("wP", "bR", ...), returns the PieceType/PieceColor it stands
    // for. Returns false (leaving outType/outColor untouched) for "." or
    // any symbol not produced by getMetadata() - keeps this the single
    // place both directions of the Symbol <-> Piece mapping are defined,
    // so callers rebuilding a snapshot from wire text never need a second
    // hand-written table.
    static bool tryParseSymbol(const std::string& symbol, PieceType& outType, PieceColor& outColor);
};