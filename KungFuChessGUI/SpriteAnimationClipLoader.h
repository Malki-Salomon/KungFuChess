#pragma once
#include <map>
#include <string>
#include "SpriteAnimationTypes.h"

// Reads every states/<name>/ subfolder under a piece's asset folder (e.g.
// ".\pictures\BB\") into a name -> SpriteAnimationClip table. Purely a
// disk -> data-structure translator: it knows the on-disk convention and
// nothing about playback or rendering. Adding a new state to the game means
// adding a states/<name>/ folder with a config.json and sprites/ - never
// changing this loader (Open/Closed).
class SpriteAnimationClipLoader
{
public:
    static std::map<std::string, SpriteAnimationClip> loadAll(const std::string& assetFolder);

private:
    static SpriteAnimationConfig loadConfig(const std::string& configJsonPath);
    static std::vector<std::string> loadFramePaths(const std::string& spritesFolder);
};
