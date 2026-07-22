#pragma once
#include <string>
#include <vector>

// Tuning knobs for one animation clip, loaded verbatim from a state's
// config.json (see pictures/<piece>/states/<name>/config.json). Pure data -
// adding a field here never touches the state machine or renderer.
struct SpriteAnimationConfig
{
    double speedMetersPerSec = 0.0;
    std::string nextStateWhenFinished; // ignored while isLoop is true
    int framesPerSec = 1;
    bool isLoop = false;
};

// A named animation clip: its config plus the ordered frame images that
// play back for it (states/<name>/sprites/1.png, 2.png, ...).
struct SpriteAnimationClip
{
    SpriteAnimationConfig config;
    std::vector<std::string> framePaths;
};
