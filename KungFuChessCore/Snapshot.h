#include "GameSnapshot.h"
#include "Board.h"

class Snapshot {
public:
    static GameSnapshot build(const Board& board);
};