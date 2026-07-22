#include "SpriteAnimationClipLoader.h"
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <algorithm>
#include <cstdlib>

namespace fs = std::filesystem;

namespace
{
    // Frames are named 1.png, 2.png, ... - sort numerically so frame 10
    // doesn't sort before frame 2.
    int frameNumber(const fs::path& path)
    {
        return std::atoi(path.stem().string().c_str());
    }
}

SpriteAnimationConfig SpriteAnimationClipLoader::loadConfig(const std::string& configJsonPath)
{
    SpriteAnimationConfig config;

    cv::FileStorage file(configJsonPath, cv::FileStorage::READ);
    if (!file.isOpened())
        return config;

    cv::FileNode physics = file["physics"];
    if (!physics.empty())
    {
        physics["speed_m_per_sec"] >> config.speedMetersPerSec;
        physics["next_state_when_finished"] >> config.nextStateWhenFinished;
    }

    cv::FileNode graphics = file["graphics"];
    if (!graphics.empty())
    {
        graphics["frames_per_sec"] >> config.framesPerSec;
        int isLoopFlag = 0;
        graphics["is_loop"] >> isLoopFlag;
        config.isLoop = isLoopFlag != 0;
    }

    return config;
}

std::vector<std::string> SpriteAnimationClipLoader::loadFramePaths(const std::string& spritesFolder)
{
    std::vector<fs::path> frameFiles;

    if (fs::exists(spritesFolder))
    {
        for (const auto& entry : fs::directory_iterator(spritesFolder))
        {
            if (entry.is_regular_file())
                frameFiles.push_back(entry.path());
        }
    }

    std::sort(frameFiles.begin(), frameFiles.end(),
        [](const fs::path& a, const fs::path& b) { return frameNumber(a) < frameNumber(b); });

    std::vector<std::string> paths;
    paths.reserve(frameFiles.size());
    for (const auto& path : frameFiles)
        paths.push_back(path.string());

    return paths;
}

std::map<std::string, SpriteAnimationClip> SpriteAnimationClipLoader::loadAll(const std::string& assetFolder)
{
    std::map<std::string, SpriteAnimationClip> clips;

    fs::path statesRoot = fs::path(assetFolder) / "states";
    if (!fs::exists(statesRoot))
        return clips;

    for (const auto& entry : fs::directory_iterator(statesRoot))
    {
        if (!entry.is_directory())
            continue;

        SpriteAnimationClip clip;
        clip.config = loadConfig((entry.path() / "config.json").string());
        clip.framePaths = loadFramePaths((entry.path() / "sprites").string());

        if (!clip.framePaths.empty())
            clips.emplace(entry.path().filename().string(), std::move(clip));
    }

    return clips;
}
