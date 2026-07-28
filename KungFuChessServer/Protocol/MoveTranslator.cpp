#include "MoveTranslator.h"

#include "MoveMessage.h"
#include "MoveNotation.h"

namespace MoveTranslator
{
    ParsedMove parseMove(const std::string& rawMessage)
    {
        ParsedMove result;

        // MoveMessage owns the message *shape* (the one place both a
        // client's build() and this parse are defined together, so they
        // can't silently drift apart) - this function only owns turning
        // already-extracted squares into board coordinates.
        std::string from, to;
        if (!MoveMessage::parse(rawMessage, from, to))
            return result;

        if (!MoveNotation::squareToPosition(from, result.fromRow, result.fromCol))
            return result;
        if (!MoveNotation::squareToPosition(to, result.toRow, result.toCol))
            return result;

        result.valid = true;
        return result;
    }

    std::vector<std::string> toClickCommands(const ParsedMove& move)
    {
        if (!move.valid)
            return {};

        return {
            MoveNotation::toClickCommand(move.fromRow, move.fromCol),
            MoveNotation::toClickCommand(move.toRow, move.toCol)
        };
    }

    std::vector<std::string> translateMoveMessage(const std::string& rawMessage)
    {
        return toClickCommands(parseMove(rawMessage));
    }
}
