#pragma once

#include "cpu_interface.h"

namespace Core
{

class IExecutionEnvironment
{
public:
    virtual ~IExecutionEnvironment() = default;

    // Returns true if simulation can continue its execution or
    // false if simulation must terminate.
    virtual bool handleEnvCall(ICpu& cpu) = 0;
};

} // namespace Core
