#include "MoveTranslator.h"

#include "MiniJson.h"
#include "MoveNotation.h"

namespace MoveTranslator
{
    ParsedMove parseMove(const std::string& rawMessage)
    {
        ParsedMove result;

        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "move")
            return result;

        std::string from, to;
        if (!MiniJson::extractStringField(rawMessage, "from", from))
            return result;
        if (!MiniJson::extractStringField(rawMessage, "to", to))
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
