#pragma once

class Board;

class Piece
{
protected:
    char color; // 'w' ае 'b'

public:
    Piece(char color);
    virtual ~Piece() = default;

    char getColor() const;

    virtual char getType() const = 0;

    virtual bool isValidMove(
        int fromRow,
        int fromCol,
        int toRow,
        int toCol,
        const Board& board) const = 0;
};

