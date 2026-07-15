#pragma once

#include <memory>

#include "PieceTypes.h"
#include "Position.h"

class Piece;

class PieceFactory
{
public:
    static std::unique_ptr<Piece> create(
        PieceType type,
        PieceColor color, Position place);
};