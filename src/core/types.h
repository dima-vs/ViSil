#pragma once
#include <cstdint>
#include "primitives.h"
#include <string_view>

namespace Core
{

struct CpuInfo
{
    std::string_view archName;
    Primitives::ArchFamily archFamily;
    uint32_t registerCount;
    Primitives::BitWidth wordWidth;
    uint32_t supportedInstrCount; // supported instruction count
};

enum class ExecutionStatus : uint16_t
{
    // === No error codes ===
    Ok = 0x0000,

    // pc changed
    PcChanged = 0x1000,
    BranchTaken,
    JumpTaken,

    // control/debug events
    ControlEvent = 0x2000,
    BreakpointHit,
    EnvCall,
    Halted,

    // === Error codes ===
    Error = 0xF000,
    IllegalInstruction,
    MemoryAccessFault,
    MisalignedAccess
};

} // namespace Core
