#include "MoveTranslator.h"

#include "MiniJson.h"
#include "MoveNotation.h"

namespace MoveTranslator
{
    std::vector<std::string> translateMoveMessage(const std::string& rawMessage)
    {
        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "move")
            return {};

        std::string from, to;
        if (!MiniJson::extractStringField(rawMessage, "from", from))
            return {};
        if (!MiniJson::extractStringField(rawMessage, "to", to))
            return {};

        int fromRow, fromCol, toRow, toCol;
        if (!MoveNotation::squareToPosition(from, fromRow, fromCol))
            return {};
        if (!MoveNotation::squareToPosition(to, toRow, toCol))
            return {};

        return {
            MoveNotation::toClickCommand(fromRow, fromCol),
            MoveNotation::toClickCommand(toRow, toCol)
        };
    }
}
