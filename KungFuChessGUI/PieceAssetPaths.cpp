#include "PieceAssetPaths.h"
#include "PieceRegistry.h"

namespace
{
    // Every PieceMetadata image path currently ends with
    // "...\states\<name>\sprites\<n>.png" - cutting at this marker leaves
    // the asset folder root.
    const std::string kStatesMarker = "states\\";
}

std::string PieceAssetPaths::getAssetFolder(PieceType type, PieceColor color)
{
    const PieceMetadata& metadata = PieceRegistry::getMetadata(type);
    const std::string& imagePath = (color == PieceColor::White) ? metadata.whiteImagePath : metadata.blackImagePath;

    auto markerPos = imagePath.find(kStatesMarker);
    return markerPos != std::string::npos ? imagePath.substr(0, markerPos) : imagePath;
}
