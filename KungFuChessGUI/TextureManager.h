#pragma once
#include <map>
#include <string>
#include <utility>
#include "img.hpp"

class TextureManager
{
public:
    // Returns the cached image for path, loading it from disk on first request.
    // size/keep_aspect follow Img::read's convention: size {} (default) keeps
    // the image at its original resolution. Note: the cache key is the path
    // alone, so requesting the same path with different size/keep_aspect
    // returns whichever was cached first.
    const Img& getTexture(const std::string& path, const std::pair<int, int>& size = {}, bool keep_aspect = false);

private:
    std::map<std::string, Img> cache;
};
