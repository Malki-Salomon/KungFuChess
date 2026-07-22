#include "BoardAnimationController.h"
#include "TextureManager.h"
#include "Layout.h"
#include "GameSnapshot.h"
#include "PieceAssetPaths.h"
#include "img.hpp"
#include <algorithm>
#include <cstdlib>

namespace
{
    struct Occupant
    {
        PieceType type;
        PieceColor color;
        int row;
        int col;
    };

    int chebyshevDistance(int rowA, int colA, int rowB, int colB)
    {
        return std::max(std::abs(rowA - rowB), std::abs(colA - colB));
    }
}

BoardAnimationController::BoardAnimationController(TextureManager& textureManager, const Layout& layout)
    : textureManager(textureManager), layout(layout)
{
}

const std::map<std::string, SpriteAnimationClip>& BoardAnimationController::clipsFor(PieceType type, PieceColor color)
{
    return catalog.getClips(PieceAssetPaths::getAssetFolder(type, color));
}

void BoardAnimationController::hintNextMoveIsJump()
{
    pendingJumpHint = true;
}

void BoardAnimationController::beginTween(TrackedPiece& piece, int fromRow, int fromCol, int toRow, int toCol)
{
    piece.fromRow = fromRow;
    piece.fromCol = fromCol;
    piece.row = toRow;
    piece.col = toCol;
    piece.isTweening = true;
    piece.tweenElapsedMs = 0.0;

    const std::string clipName = pendingJumpHint ? "jump" : "move";
    pendingJumpHint = false; // consumed by this move, whether or not it was the intended one

    piece.stateMachine.requestState(clipName);

    // Duration is derived from the clip's own authored speed_m_per_sec
    // (already loaded as part of this state's config) so a piece "walks" at
    // the pace its own animation data implies, rather than a duration
    // guessed independently of the sprite data.
    const auto& clips = clipsFor(piece.type, piece.color);
    auto clipIt = clips.find(clipName);
    double speedMetersPerSec = (clipIt != clips.end() && clipIt->second.config.speedMetersPerSec > 0.0)
        ? clipIt->second.config.speedMetersPerSec : 1.0;

    int distanceCells = std::max(1, chebyshevDistance(fromRow, fromCol, toRow, toCol));
    piece.tweenDurationMs = (distanceCells / speedMetersPerSec) * 1000.0;
}

void BoardAnimationController::onSnapshotReceived(const GameSnapshot& snapshot)
{
    std::vector<Occupant> occupants;
    for (int row = 0; row < static_cast<int>(snapshot.cells.size()); ++row)
    {
        for (int col = 0; col < static_cast<int>(snapshot.cells[row].size()); ++col)
        {
            const PieceSnapshot& cell = snapshot.cells[row][col];
            if (cell.type != PieceType::Empty)
                occupants.push_back({ cell.type, cell.color, row, col });
        }
    }

    std::vector<bool> pieceMatched(pieces.size(), false);
    std::vector<bool> occupantMatched(occupants.size(), false);

    // Pass 1: pieces already exactly where the snapshot says they are (or
    // already mid-tween toward that cell) need no reaction at all.
    for (size_t i = 0; i < pieces.size(); ++i)
    {
        for (size_t j = 0; j < occupants.size(); ++j)
        {
            if (occupantMatched[j])
                continue;

            const Occupant& occ = occupants[j];
            if (pieces[i].type == occ.type && pieces[i].color == occ.color &&
                pieces[i].row == occ.row && pieces[i].col == occ.col)
            {
                pieceMatched[i] = true;
                occupantMatched[j] = true;
                break;
            }
        }
    }

    // Pass 2: pair up remaining departed pieces with remaining arrived
    // occupants of the same type+color, nearest first - each pairing IS a
    // detected move and becomes a tween.
    struct Candidate { size_t pieceIndex; size_t occupantIndex; int distance; };
    std::vector<Candidate> candidates;
    for (size_t i = 0; i < pieces.size(); ++i)
    {
        if (pieceMatched[i])
            continue;

        for (size_t j = 0; j < occupants.size(); ++j)
        {
            if (occupantMatched[j])
                continue;

            const Occupant& occ = occupants[j];
            if (pieces[i].type != occ.type || pieces[i].color != occ.color)
                continue;

            candidates.push_back({ i, j, chebyshevDistance(pieces[i].row, pieces[i].col, occ.row, occ.col) });
        }
    }

    std::sort(candidates.begin(), candidates.end(),
        [](const Candidate& a, const Candidate& b) { return a.distance < b.distance; });

    for (const Candidate& candidate : candidates)
    {
        if (pieceMatched[candidate.pieceIndex] || occupantMatched[candidate.occupantIndex])
            continue;

        TrackedPiece& piece = pieces[candidate.pieceIndex];
        const Occupant& occ = occupants[candidate.occupantIndex];
        beginTween(piece, piece.row, piece.col, occ.row, occ.col);

        pieceMatched[candidate.pieceIndex] = true;
        occupantMatched[candidate.occupantIndex] = true;
    }

    // Pass 3: unmatched departures - that piece is gone (captured, or
    // replaced in place by promotion, which pass 4 will spawn fresh). It
    // won't be in `pieces` for draw() to erase itself anymore, so queue its
    // last-drawn spot for erasure here instead.
    int cellSize = layout.getCellSize();
    for (size_t i = pieces.size(); i-- > 0; )
    {
        if (!pieceMatched[i])
        {
            const TrackedPiece& removed = pieces[i];
            if (removed.hasBeenDrawn)
                pendingErasures.push_back({ removed.lastDrawnX, removed.lastDrawnY, cellSize, cellSize });

            pieces.erase(pieces.begin() + i);
        }
    }

    // Pass 4: unmatched arrivals - a brand-new piece with no prior tracked
    // state (initial board load, promotion). Starts idle immediately rather
    // than tweening in from nowhere.
    for (size_t j = 0; j < occupants.size(); ++j)
    {
        if (occupantMatched[j])
            continue;

        const Occupant& occ = occupants[j];
        pieces.push_back(TrackedPiece{ occ.type, occ.color, occ.row, occ.col,
            SpriteAnimationStateMachine(clipsFor(occ.type, occ.color), "idle") });
    }
}

void BoardAnimationController::update(long long deltaMs)
{
    for (TrackedPiece& piece : pieces)
    {
        if (piece.isTweening)
        {
            piece.tweenElapsedMs += static_cast<double>(deltaMs);

            if (piece.tweenElapsedMs >= piece.tweenDurationMs)
            {
                piece.isTweening = false;
                piece.fromRow = piece.row;
                piece.fromCol = piece.col;

                // Settle into whatever the just-played clip's own config
                // says comes next (move -> long_rest, jump already resolved
                // itself to short_rest mid-flight) instead of hardcoding
                // "idle", so behavior stays data-driven off the same
                // config.json the clip was loaded from.
                const auto& clips = clipsFor(piece.type, piece.color);
                auto clipIt = clips.find(piece.stateMachine.getCurrentState());
                const std::string settleState =
                    (clipIt != clips.end() && !clipIt->second.config.nextStateWhenFinished.empty())
                    ? clipIt->second.config.nextStateWhenFinished : "idle";

                piece.stateMachine.requestState(settleState);
            }
        }

        piece.stateMachine.update(deltaMs);
    }
}

void BoardAnimationController::draw(Img& canvas, const Img& boardBackground)
{
    int cellSize = layout.getCellSize();

    // Erase pieces that were captured/replaced since the last draw() -
    // they're already gone from `pieces`, so this is the only remaining
    // chance to clean up their old sprite.
    for (const ErasedRegion& region : pendingErasures)
    {
        Img patch = boardBackground.crop(region.x, region.y, region.width, region.height);
        patch.draw_on(canvas, region.x, region.y);
    }
    pendingErasures.clear();

    for (TrackedPiece& piece : pieces)
    {
        cv::Rect targetRect = layout.getCellRect(piece.row, piece.col);
        int drawX = targetRect.x;
        int drawY = targetRect.y;

        if (piece.isTweening)
        {
            double t = piece.tweenDurationMs > 0.0
                ? std::min(1.0, piece.tweenElapsedMs / piece.tweenDurationMs) : 1.0;

            cv::Rect fromRect = layout.getCellRect(piece.fromRow, piece.fromCol);
            drawX = fromRect.x + static_cast<int>((targetRect.x - fromRect.x) * t);
            drawY = fromRect.y + static_cast<int>((targetRect.y - fromRect.y) * t);
        }

        const std::string& framePath = piece.stateMachine.getCurrentFramePath();

        bool unchanged = piece.hasBeenDrawn && drawX == piece.lastDrawnX && drawY == piece.lastDrawnY
            && framePath == piece.lastDrawnFramePath;
        if (unchanged)
            continue; // static: the canvas already shows this piece correctly, skip entirely

        if (piece.hasBeenDrawn)
        {
            // The piece moved (mid-tween) or advanced to a different
            // frame: erase its previous spot before painting the new one.
            Img patch = boardBackground.crop(piece.lastDrawnX, piece.lastDrawnY, cellSize, cellSize);
            patch.draw_on(canvas, piece.lastDrawnX, piece.lastDrawnY);
        }

        if (!framePath.empty())
        {
            Img frameImg = textureManager.getTexture(framePath, { cellSize, cellSize }, true);
            frameImg.draw_on(canvas, drawX, drawY);
        }

        piece.hasBeenDrawn = true;
        piece.lastDrawnX = drawX;
        piece.lastDrawnY = drawY;
        piece.lastDrawnFramePath = framePath;
    }
}

void BoardAnimationController::forceFullRedraw()
{
    for (TrackedPiece& piece : pieces)
        piece.hasBeenDrawn = false;

    // The canvas these erasures targeted no longer exists in its old form.
    pendingErasures.clear();
}
