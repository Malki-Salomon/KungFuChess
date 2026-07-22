#pragma once
#include <cstddef>
#include <map>
#include <string>
#include "SpriteAnimationTypes.h"

// Drives a single sprite's playback: which named state is active, which
// frame within it, and how far into that frame we are. Knows nothing about
// pixels, textures, or the screen - only the clip table it is given and the
// passage of time (update). Keeping this ignorant of rendering is what lets
// the state logic be reasoned about independently of how frames end up on
// screen - its Single Responsibility is "what should be showing right now",
// not "how to paint it" or "where on screen".
//
// Holds `clips` as a pointer rather than a reference so this type stays
// copy/move-assignable - it lives inside a std::vector<TrackedPiece> in
// BoardAnimationController, whose erase()/reshuffling needs assignment.
class SpriteAnimationStateMachine
{
public:
    // The table pointed to by `clips` must outlive this state machine; it
    // is owned by a SpriteAnimationCatalog entry that lives for the whole
    // program run.
    SpriteAnimationStateMachine(const std::map<std::string, SpriteAnimationClip>& clips,
                                 const std::string& initialState);

    // Switches to `state` immediately, restarting playback from frame 0.
    // No-op if `state` is already current or isn't a known clip name, so
    // callers can call this every tick without guarding the state name
    // themselves.
    void requestState(const std::string& state);

    // Advances playback by deltaMs: steps frames per the active clip's
    // frames_per_sec, then either loops, holds on the last frame, or - for
    // one-shot clips with next_state_when_finished set - transitions there.
    void update(long long deltaMs);

    const std::string& getCurrentState() const { return currentState; }
    const std::string& getCurrentFramePath() const;

private:
    const SpriteAnimationClip& currentClip() const;
    void advanceFrame();

    const std::map<std::string, SpriteAnimationClip>* clips;
    std::string currentState;
    std::size_t frameIndex = 0;
    double elapsedMsInFrame = 0.0;
};
