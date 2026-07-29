#pragma once

#include "PlayerAssignment.h"
#include "PieceTypes.h"

#include <string>

// The one place that decides what a Seat *means* in this specific game
// (PlayerAssignment itself knows nothing about chess/colors - see its
// header) and what a color is called on the wire. Shared by Server.cpp
// (the move-ownership check) and AuthWorker.cpp (building the "assigned"
// message on a successful login) so neither duplicates the other's copy.
namespace SeatColorNaming
{
    PieceColor colorForSeat(Seat seat);
    std::string colorName(PieceColor color);
}
