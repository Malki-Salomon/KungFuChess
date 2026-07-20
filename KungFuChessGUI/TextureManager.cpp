#include "TextureManager.h"

const Img& TextureManager::getTexture(const std::string& path, const std::pair<int, int>& size, bool keep_aspect)
{
    auto it = cache.find(path);
    if (it != cache.end())
        return it->second;

    Img img;
    img.read(path, size, keep_aspect);
    return cache.emplace(path, img).first->second;
}
