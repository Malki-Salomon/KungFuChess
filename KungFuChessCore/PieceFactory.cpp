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
    PieceColor color, Position place)
{
    switch (type)
    {
    case PieceType::King:
        return make_unique<King>(color, place, type);

    case PieceType::Queen:
        return make_unique<Queen>(color, place, type);

    case PieceType::Rook:
        return make_unique<Rook>(color, place, type);

    case PieceType::Bishop:
        return make_unique<Bishop>(color, place, type);

    case PieceType::Knight:
        return make_unique<Knight>(color, place, type);

    case PieceType::Pawn:
        return make_unique<Pawn>(color, place, type);

    default:
        return nullptr;
    }
}