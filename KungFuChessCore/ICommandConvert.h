#pragma once

#include "Command.h"
#include <vector>
#include <memory>

class ICommandConvert
{
public:
    virtual ~ICommandConvert() = default;

    virtual std::vector<std::unique_ptr<Command>>
        Convert() = 0;
};