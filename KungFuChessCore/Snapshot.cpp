#include "pch.h"
#include "Snapshot.h"

GameSnapshot Snapshot::build(const Board& board) {
    GameSnapshot snapshot;
    snapshot.cells.resize(board.getRows());
    for (auto& row : snapshot.cells) {
        row.resize(board.getCols());
    }

    for (int r = 0; r < snapshot.cells.size(); ++r) {
        for (int c = 0; c < snapshot.cells[0].size(); ++c) {
            Position pos(r, c);
            if (!board.isEmpty(pos)) {
                snapshot.cells[r][c] = { board.getPiece(pos)->getType(), board.getPiece(pos)->getColor() };
            }
            else {
                snapshot.cells[r][c] = { PieceType::Empty, PieceColor::None };
            }
        }
    }
    return snapshot;
}