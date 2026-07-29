#include "SeatColorNaming.h"

namespace SeatColorNaming
{
    PieceColor colorForSeat(Seat seat)
    {
        switch (seat)
        {
            case Seat::First:  return PieceColor::White;
            case Seat::Second: return PieceColor::Black;
            default:           return PieceColor::None; // spectator
        }
    }

    std::string colorName(PieceColor color)
    {
        switch (color)
        {
            case PieceColor::White: return "white";
            case PieceColor::Black: return "black";
            default:                return "spectator";
        }
    }
}
