#include "common.h"
#include <QString>

namespace Gui
{

enum class MemInstrRole
{
    DisassembledInstrRole = Qt::UserRole + 1,
    BreakPointRole,
    AddressRole
};

enum class MemInstrColumn
{
    Breakpoint = 0,
    Address,
    Disassembly,
    Count
};

} // namespace Gui
