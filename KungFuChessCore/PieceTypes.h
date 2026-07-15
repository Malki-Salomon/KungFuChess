#pragma once

enum class PieceType
{
    Empty,
    King,
    Queen,
    Rook,
    Bishop,
    Knight,
    Pawn
};

enum class PieceColor
{
    White,
    Black,
    None
};

enum class PieceStatus
{
    idle,
    moving,
    captured,
    airborne
};