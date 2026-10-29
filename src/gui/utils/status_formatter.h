#pragma once

#include <QString>
#include <QPair>
#include "core/cpu_interface.h"

namespace Gui
{

class StatusFormatter
{
public:
    // return pair: <StatusCode, StatusMessage>
    static QPair<QString, QString> formatStatus(Core::ExecutionStatus status);
};

} // namespace Gui
