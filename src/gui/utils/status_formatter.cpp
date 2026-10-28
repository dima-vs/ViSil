#include "status_formatter.h"

namespace Gui
{

QPair<QString, QString> StatusFormatter::formatStatus(Core::ExecutionStatus status)
{
    using Core::ExecutionStatus;

    switch (status)
    {
    // === No error ===
    case ExecutionStatus::Ok:
        return {"Ok", "No error"};

    // === Execution-flow events ===
    case ExecutionStatus::PcChanged:
        return {"PcChanged", "Program counter changed"};

    case ExecutionStatus::BranchTaken:
        return {"BranchTaken", "Branch taken"};

    case ExecutionStatus::JumpTaken:
        return {"JumpTaken", "Jump taken"};

    // === Control & Debug events ===
    case ExecutionStatus::ControlEvent:
        return {"ControlEvent", "Control event"};

    case ExecutionStatus::BreakpointHit:
        return {"BreakpointHit", "Breakpoint reached"};

    case ExecutionStatus::EnvCall:
        return {"EnvCall", "Environment call executed"};

    case ExecutionStatus::Halted:
        return {"Halted", "CPU halted"};

    // === Errors ===
    case ExecutionStatus::Error:
        return {"Error", "Unspecified execution error"};

    case ExecutionStatus::IllegalInstruction:
        return {"IllegalInstruction", "Illegal instruction encountered"};

    case ExecutionStatus::MemoryAccessFault:
        return {"MemoryAccessFault", "Memory access fault"};

    case ExecutionStatus::MisalignedAccess:
        return {"MisalignedAccess", "Misaligned memory access"};

    default:
        return {
            "UnknownStatus",
            QString("Unknown execution status (Code: 0x%1)")
                .arg(QString::number(static_cast<quint16>(status), 16).toUpper(), 4, QChar('0'))
        };
    }
}

} // namespace Gui
