#pragma once
#include <cstdint>

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

enum class ExecutionStatus
{
    Ok,
    Halted,
    BreakpointHit,
    IllegalInstruction,
    MemoryAccessFault
};

} // namespace Core
