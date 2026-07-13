#include "pch.h"
#include "PieceFactory.h"

#include "King.h"
#include "Queen.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"
#include "Pawn.h"

using namespace std;

unique_ptr<Piece> PieceFactory::create(
    PieceType type,
    PieceColor color)
{
    switch (type)
    {
    case PieceType::King:
        return make_unique<King>(color);

    case PieceType::Queen:
        return make_unique<Queen>(color);

    case PieceType::Rook:
        return make_unique<Rook>(color);

    case PieceType::Bishop:
        return make_unique<Bishop>(color);

    case PieceType::Knight:
        return make_unique<Knight>(color);

    case PieceType::Pawn:
        return make_unique<Pawn>(color);

    default:
        return nullptr;
    }
}