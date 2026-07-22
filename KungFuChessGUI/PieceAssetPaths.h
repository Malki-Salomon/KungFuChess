#pragma once
#include <string>
#include "PieceTypes.h"

// Derives a piece's animation asset folder (e.g. ".\pictures\BB\") purely
// from Core's existing, unmodified PieceRegistry::getMetadata() image path -
// Core exposes nothing new for this. We just know its on-disk convention
// ("...\states\<name>\sprites\<n>.png") and strip that known suffix off the
// idle image path it already hands back.
namespace PieceAssetPaths
{
    std::string getAssetFolder(PieceType type, PieceColor color);
}
