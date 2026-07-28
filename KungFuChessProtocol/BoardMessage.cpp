#include "BoardMessage.h"

#include "MiniJson.h"

#include <sstream>

namespace BoardMessage
{
    std::string build(const std::vector<std::vector<std::string>>& cells)
    {
        std::ostringstream out;
        out << "{\"type\":\"board\",\"cells\":[";

        for (size_t row = 0; row < cells.size(); ++row)
        {
            if (row > 0)
                out << ",";
            out << "[";

            const auto& rowCells = cells[row];
            for (size_t col = 0; col < rowCells.size(); ++col)
            {
                if (col > 0)
                    out << ",";
                out << "\"" << rowCells[col] << "\"";
            }

            out << "]";
        }

        out << "]}";
        return out.str();
    }

    bool parse(const std::string& rawMessage, std::vector<std::vector<std::string>>& outRows)
    {
        std::string type;
        if (!MiniJson::extractStringField(rawMessage, "type", type) || type != "board")
            return false;

        size_t cellsPos = rawMessage.find("\"cells\"");
        if (cellsPos == std::string::npos)
            return false;

        // Collect every quoted string token from "cells" onward, in order.
        std::vector<std::string> tokens;
        size_t pos = cellsPos;
        while (true)
        {
            size_t firstQuote = rawMessage.find('"', pos);
            if (firstQuote == std::string::npos)
                break;
            size_t secondQuote = rawMessage.find('"', firstQuote + 1);
            if (secondQuote == std::string::npos)
                break;

            tokens.push_back(rawMessage.substr(firstQuote + 1, secondQuote - firstQuote - 1));
            pos = secondQuote + 1;
        }

        // The very first token collected is the literal word "cells"
        // itself (from `"cells":`), not a board symbol - drop it.
        if (tokens.empty())
            return false;
        tokens.erase(tokens.begin());

        if (tokens.size() != 64)
            return false;

        outRows.assign(8, std::vector<std::string>(8));
        for (int row = 0; row < 8; ++row)
        {
            for (int col = 0; col < 8; ++col)
            {
                outRows[row][col] = tokens[row * 8 + col];
            }
        }

        return true;
    }
}
