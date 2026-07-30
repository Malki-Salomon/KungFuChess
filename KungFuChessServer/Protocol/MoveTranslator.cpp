#include "MoveTranslator.h"

#include "MoveMessage.h"
#include "JumpMessage.h"
#include "MoveNotation.h"
#include "SquareNotation.h"

namespace MoveTranslator
{
    ParsedMove parseMove(const std::string& rawMessage)
    {
        ParsedMove result;

        // Responsibilities are split three ways here, none of them this
        // function's own: MoveMessage owns the message *shape*,
        // Protocol::SquareNotation owns the square <-> coordinate
        // convention (shared with every client, so it can't drift), and
        // this function only assembles the result.
        std::string from, to;
        if (!MoveMessage::parse(rawMessage, from, to))
            return result;

        Protocol::Square fromSquare{};
        Protocol::Square toSquare{};
        if (!Protocol::SquareNotation::tryParse(from, fromSquare))
            return result;
        if (!Protocol::SquareNotation::tryParse(to, toSquare))
            return result;

        result.fromRow = fromSquare.row;
        result.fromCol = fromSquare.col;
        result.toRow = toSquare.row;
        result.toCol = toSquare.col;
        result.valid = true;
        return result;
    }

    ParsedJump parseJump(const std::string& rawMessage)
    {
        ParsedJump result;

        // Same split of responsibilities as parseMove above.
        std::string square;
        if (!JumpMessage::parse(rawMessage, square))
            return result;

        Protocol::Square parsedSquare{};
        if (!Protocol::SquareNotation::tryParse(square, parsedSquare))
            return result;

        result.row = parsedSquare.row;
        result.col = parsedSquare.col;
        result.valid = true;
        return result;
    }

    std::string toJumpCommand(const ParsedJump& jump)
    {
        if (!jump.valid)
            return {};

        return MoveNotation::toJumpCommand(jump.row, jump.col);
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
