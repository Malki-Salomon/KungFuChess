#pragma once
#include <vector>
#include "PieceTypes.h"
#include "SpriteAnimationCatalog.h"
#include "SpriteAnimationStateMachine.h"
#include "IMoveIntentHint.h"

class Img;
class TextureManager;
class Layout;
struct GameSnapshot;

// Turns the raw {type,color} grids Core hands over into animated on-screen
// pieces, entirely on the GUI side. Core is never told this exists: this
// class treats GameSnapshot as an opaque "what does the board look like
// right now" signal and reconstructs everything else itself - which piece
// moved where, how long that should take to animate, when a piece is
// merely idling - by diffing consecutive snapshots against its own
// remembered piece list. This is the same trick online chess clients use to
// animate a move a server only ever reports as already-completed.
class BoardAnimationController : public IMoveIntentHint
{
public:
    BoardAnimationController(TextureManager& textureManager, const Layout& layout);

    // Reconciles the internally tracked pieces against a freshly arrived
    // snapshot: pairs up disappeared/appeared cells of matching type+color
    // into move tweens, spawns idle pieces for arrivals with no match
    // (initial setup, promotions), and drops pieces with no match at all
    // (captures).
    void onSnapshotReceived(const GameSnapshot& snapshot);

    // Advances every tracked piece's tween progress and sprite playback by
    // deltaMs. Call this every GUI loop tick, independent of how often
    // onSnapshotReceived fires, so idle/looping clips stay smooth even
    // between Core-driven updates.
    void update(long long deltaMs);

    // Repaints only what changed since the last call: a piece whose pixel
    // position and current frame are both identical to last time is left
    // untouched entirely (no texture lookup, no draw_on). A piece that
    // moved or advanced to a new frame has its previous spot patched with
    // `boardBackground` first (erasing the old sprite), then its current
    // frame drawn. `boardBackground` must be the same size/position as
    // `canvas` - see BoardRenderer::getTexture().
    void draw(Img& canvas, const Img& boardBackground);

    // Marks every tracked piece as needing a full repaint on the next
    // draw() call. Call this whenever `canvas` has been recreated from
    // scratch (e.g. board resized) - otherwise this class's "already drawn
    // here" bookkeeping would wrongly skip pieces that are no longer
    // actually on the new canvas.
    void forceFullRedraw();

    // IMoveIntentHint
    void hintNextMoveIsJump() override;

private:
    struct TrackedPiece
    {
        PieceType type;
        PieceColor color;
        int row;
        int col;                    // logical/destination cell
        SpriteAnimationStateMachine stateMachine;

        bool isTweening = false;
        int fromRow = 0;
        int fromCol = 0;
        double tweenElapsedMs = 0.0;
        double tweenDurationMs = 0.0;

        // What draw() last actually painted for this piece, so it can tell
        // "nothing changed, skip" apart from "moved or changed frame,
        // repaint" and knows what to erase when it does the latter.
        bool hasBeenDrawn = false;
        int lastDrawnX = 0;
        int lastDrawnY = 0;
        std::string lastDrawnFramePath;
    };

    // A screen-space square queued for erasure (repainting with the board
    // background) on the next draw() call - used for pieces removed from
    // `pieces` (captured) between one draw() and the next, since draw()'s
    // per-piece loop can no longer see them to erase their old spot itself.
    struct ErasedRegion { int x; int y; int width; int height; };

    const std::map<std::string, SpriteAnimationClip>& clipsFor(PieceType type, PieceColor color);
    void beginTween(TrackedPiece& piece, int fromRow, int fromCol, int toRow, int toCol);

    TextureManager& textureManager;
    const Layout& layout;
    SpriteAnimationCatalog catalog;
    std::vector<TrackedPiece> pieces;
    std::vector<ErasedRegion> pendingErasures;
    bool pendingJumpHint = false;
};
