#include "SpriteAnimationCatalog.h"
#include "SpriteAnimationClipLoader.h"

const std::map<std::string, SpriteAnimationClip>& SpriteAnimationCatalog::getClips(const std::string& assetFolder)
{
    auto it = cache.find(assetFolder);
    if (it != cache.end())
        return it->second;

    return cache.emplace(assetFolder, SpriteAnimationClipLoader::loadAll(assetFolder)).first->second;
}
