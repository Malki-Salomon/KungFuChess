#pragma once
#include <map>
#include <string>
#include "SpriteAnimationTypes.h"

// Caches the on-disk clip tables per asset folder, so each piece type/color's
// config.json + sprite lists are parsed once no matter how many instances of
// that piece are on the board. Mirrors TextureManager's caching role one
// layer up: TextureManager caches decoded images, this caches which images
// and timings belong to which named state.
class SpriteAnimationCatalog
{
public:
    const std::map<std::string, SpriteAnimationClip>& getClips(const std::string& assetFolder);

private:
    std::map<std::string, std::map<std::string, SpriteAnimationClip>> cache;
};
