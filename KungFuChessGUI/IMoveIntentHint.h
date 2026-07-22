#pragma once

// Narrow, GUI-only interface: lets InputHandler tell the animation system,
// at the moment it dispatches a jump command, that the next board change it
// detects should play the "jump" clip instead of the default "move" clip.
// Core has no notion of this - it's a hint passed between two GUI classes
// that would otherwise have no way to tell a jump from a slide, since the
// {type,color} snapshot Core hands over doesn't say. Best-effort: if the
// click turns out illegal and no board change follows, the hint just goes
// unused and is discarded on the next one.
class IMoveIntentHint
{
public:
    virtual ~IMoveIntentHint() = default;
    virtual void hintNextMoveIsJump() = 0;
};
