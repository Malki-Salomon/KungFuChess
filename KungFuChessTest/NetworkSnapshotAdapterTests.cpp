#include "doctest.h"

#include "NetworkSnapshotAdapter.h"

#include "Events/EventBus.h"
#include "Events/GameStateChangedEvent.h"
#include "GameSnapshot.h"

#include "BoardMessage.h"
#include "GameOverMessage.h"
#include "AssignedMessage.h"

#include <string>
#include <vector>

namespace
{
    // The standard starting position as the wire format carries it: row 0
    // is black's back rank, row 7 is white's.
    std::vector<std::vector<std::string>> startingCells()
    {
        return {
            { "bR", "bN", "bB", "bQ", "bK", "bB", "bN", "bR" },
            { "bP", "bP", "bP", "bP", "bP", "bP", "bP", "bP" },
            { ".",  ".",  ".",  ".",  ".",  ".",  ".",  "."  },
            { ".",  ".",  ".",  ".",  ".",  ".",  ".",  "."  },
            { ".",  ".",  ".",  ".",  ".",  ".",  ".",  "."  },
            { ".",  ".",  ".",  ".",  ".",  ".",  ".",  "."  },
            { "wP", "wP", "wP", "wP", "wP", "wP", "wP", "wP" },
            { "wR", "wN", "wB", "wQ", "wK", "wB", "wN", "wR" },
        };
    }
}

TEST_SUITE("NetworkSnapshotAdapter")
{
    TEST_CASE("publishes a fully reconstructed snapshot for a valid board")
    {
        EventBus bus;

        int publishCount = 0;
        GameSnapshot received;
        bus.subscribe<GameStateChangedEvent>([&](const GameStateChangedEvent& event)
        {
            ++publishCount;
            received = event.getSnapshot();
        });

        NetworkSnapshotAdapter adapter(bus);
        adapter.onMessage(BoardMessage::build(startingCells()));

        REQUIRE(publishCount == 1);
        REQUIRE(received.cells.size() == 8);
        REQUIRE(received.cells[0].size() == 8);

        // Spot-check each distinct piece kind, both colors, plus an empty.
        CHECK(received.cells[0][0].type == PieceType::Rook);
        CHECK(received.cells[0][0].color == PieceColor::Black);
        CHECK(received.cells[0][3].type == PieceType::Queen);
        CHECK(received.cells[0][4].type == PieceType::King);
        CHECK(received.cells[1][5].type == PieceType::Pawn);
        CHECK(received.cells[1][5].color == PieceColor::Black);

        CHECK(received.cells[7][0].type == PieceType::Rook);
        CHECK(received.cells[7][0].color == PieceColor::White);
        CHECK(received.cells[7][1].type == PieceType::Knight);
        CHECK(received.cells[7][2].type == PieceType::Bishop);
        CHECK(received.cells[6][4].type == PieceType::Pawn);
        CHECK(received.cells[6][4].color == PieceColor::White);

        CHECK(received.cells[4][4].type == PieceType::Empty);
        CHECK(received.cells[4][4].color == PieceColor::None);
    }

    // The central rule of principle 4: a board message carrying even one
    // unparseable symbol must be discarded whole. A partially-correct
    // snapshot reaching the renderer would be worse than no update at all.
    TEST_CASE("never publishes anything for a board with one invalid symbol")
    {
        EventBus bus;

        int publishCount = 0;
        bus.subscribe<GameStateChangedEvent>([&](const GameStateChangedEvent&)
        {
            ++publishCount;
        });

        NetworkSnapshotAdapter adapter(bus);

        auto cells = startingCells();
        cells[4][2] = "wZ"; // no such piece

        adapter.onMessage(BoardMessage::build(cells));

        CHECK(publishCount == 0);
    }

    // The bad cell sits at the very end, after 63 perfectly good ones -
    // proving the discard isn't just "we bailed before publishing the
    // first cell", but that a whole snapshot's worth of valid work is
    // genuinely thrown away.
    TEST_CASE("discards the whole snapshot even when only the last cell is invalid")
    {
        EventBus bus;

        int publishCount = 0;
        bus.subscribe<GameStateChangedEvent>([&](const GameStateChangedEvent&)
        {
            ++publishCount;
        });

        NetworkSnapshotAdapter adapter(bus);

        auto cells = startingCells();
        cells[7][7] = "??";

        adapter.onMessage(BoardMessage::build(cells));

        CHECK(publishCount == 0);
    }

    TEST_CASE("a discarded board does not corrupt the next valid one")
    {
        EventBus bus;

        int publishCount = 0;
        GameSnapshot received;
        bus.subscribe<GameStateChangedEvent>([&](const GameStateChangedEvent& event)
        {
            ++publishCount;
            received = event.getSnapshot();
        });

        NetworkSnapshotAdapter adapter(bus);

        auto bad = startingCells();
        bad[0][0] = "xx";
        adapter.onMessage(BoardMessage::build(bad));
        CHECK(publishCount == 0);

        adapter.onMessage(BoardMessage::build(startingCells()));
        REQUIRE(publishCount == 1);
        CHECK(received.cells[0][0].type == PieceType::Rook);
        CHECK(received.cells[0][0].color == PieceColor::Black);
    }

    // Non-board messages are handled, but must never masquerade as a board
    // update - nothing about a gameOver or assigned message tells the
    // renderer where pieces are.
    TEST_CASE("non-board messages publish no board state")
    {
        EventBus bus;

        int publishCount = 0;
        bus.subscribe<GameStateChangedEvent>([&](const GameStateChangedEvent&)
        {
            ++publishCount;
        });

        NetworkSnapshotAdapter adapter(bus);

        adapter.onMessage(GameOverMessage::build("white", true, 1216, 1184));
        adapter.onMessage(AssignedMessage::build("black"));

        CHECK(publishCount == 0);
    }

    TEST_CASE("unrecognized and malformed messages are ignored without throwing")
    {
        EventBus bus;

        int publishCount = 0;
        bus.subscribe<GameStateChangedEvent>([&](const GameStateChangedEvent&)
        {
            ++publishCount;
        });

        NetworkSnapshotAdapter adapter(bus);

        CHECK_NOTHROW(adapter.onMessage(""));
        CHECK_NOTHROW(adapter.onMessage("not json at all"));
        CHECK_NOTHROW(adapter.onMessage("{\"type\":\"somethingBrandNew\"}"));
        CHECK_NOTHROW(adapter.onMessage("{\"type\":\"board\"}")); // no cells
        CHECK(publishCount == 0);
    }
}
