#include "SpriteAnimationStateMachine.h"

namespace
{
    // Fallback used when the current state name has no matching clip (e.g.
    // the asset folder failed to load) so lookups stay safe instead of
    // needing to throw or crash mid-frame.
    const SpriteAnimationClip& emptyClip()
    {
        static const SpriteAnimationClip empty;
        return empty;
    }

    const std::string& emptyPath()
    {
        static const std::string empty;
        return empty;
    }
}

SpriteAnimationStateMachine::SpriteAnimationStateMachine(const std::map<std::string, SpriteAnimationClip>& clips,
                                                          const std::string& initialState)
    : clips(&clips), currentState(initialState)
{
}

void SpriteAnimationStateMachine::requestState(const std::string& state)
{
    if (state == currentState || clips->find(state) == clips->end())
        return;

    currentState = state;
    frameIndex = 0;
    elapsedMsInFrame = 0.0;
}

const SpriteAnimationClip& SpriteAnimationStateMachine::currentClip() const
{
    auto it = clips->find(currentState);
    return it != clips->end() ? it->second : emptyClip();
}

void SpriteAnimationStateMachine::advanceFrame()
{
    const SpriteAnimationClip& clip = currentClip();
    if (clip.framePaths.empty())
        return;

    ++frameIndex;

    if (frameIndex < clip.framePaths.size())
        return;

    if (clip.config.isLoop)
    {
        frameIndex = 0;
        return;
    }

    // One-shot clip finished: hold on the last frame and follow the
    // configured transition, if any and if it names a known state.
    frameIndex = clip.framePaths.size() - 1;

    if (!clip.config.nextStateWhenFinished.empty())
        requestState(clip.config.nextStateWhenFinished);
}

void SpriteAnimationStateMachine::update(long long deltaMs)
{
    elapsedMsInFrame += static_cast<double>(deltaMs);

    // Loop rather than a single step so a lagging frame (large deltaMs)
    // still advances the correct number of frames instead of stalling.
    for (;;)
    {
        const SpriteAnimationClip& clip = currentClip();
        if (clip.framePaths.empty() || clip.config.framesPerSec <= 0)
            break;

        const double msPerFrame = 1000.0 / clip.config.framesPerSec;
        if (elapsedMsInFrame < msPerFrame)
            break;

        elapsedMsInFrame -= msPerFrame;
        advanceFrame(); // may switch state, so the clip is re-read next iteration
    }
}

const std::string& SpriteAnimationStateMachine::getCurrentFramePath() const
{
    const SpriteAnimationClip& clip = currentClip();
    if (frameIndex >= clip.framePaths.size())
        return emptyPath();

    return clip.framePaths[frameIndex];
}
