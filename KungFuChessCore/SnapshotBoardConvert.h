#pragma once
#include "GameSnapshot.h"
#include "IBoardConvert.h"

class SnapshotBoardConvert : public IBoardConvert
{
public:
    SnapshotBoardConvert(const GameSnapshot& snapshot);

    bool Convert(Board& board) override;

private:
    GameSnapshot snapshot;
};
