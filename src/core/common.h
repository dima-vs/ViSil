#pragma once

#include <cstdint>
#include <string_view>

namespace Core
{

enum class Endianness : uint8_t
{
    Little,
    Big
};

enum class BitWidth : uint16_t
{
    Bits8   = 8,
    Bits16  = 16,
    Bits32  = 32,
    Bits64  = 64,
    Bits128 = 128
};

enum class ArchFamily : uint8_t
{
    Risc,
    Cisc,
    Other
};

enum class BusStatus : uint8_t
{
    Ok,
    UnmappedAddress,
    AccessFault,
    MisalignedAccess
};

enum class ExecutionStatus : uint16_t
{
    // === No error codes ===
    Ok = 0x0000,

    // PC changed
    PcChanged = 0x1000,
    BranchTaken,
    JumpTaken,

    // Control & Debug events
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

struct CpuInfo
{
    std::string_view archName;
    ArchFamily archFamily;
    uint32_t registerCount;
    BitWidth wordWidth;
    uint32_t supportedInstrCount;
};

constexpr ExecutionStatus mapBusStatusToExecStatus(BusStatus status) noexcept
{
    switch (status)
    {
    case BusStatus::Ok:
        return ExecutionStatus::Ok;
    case BusStatus::MisalignedAccess:
        return ExecutionStatus::MisalignedAccess;
    case BusStatus::UnmappedAddress:
    case BusStatus::AccessFault:
        return ExecutionStatus::MemoryAccessFault;
    }
    return ExecutionStatus::Error;
}

constexpr bool isOk(ExecutionStatus status) noexcept
{
    return status == ExecutionStatus::Ok;
}

constexpr bool isPcChanged(ExecutionStatus status) noexcept
{
    const auto val = static_cast<uint16_t>(status);
    return val >= 0x1000 && val < 0x2000;
}

constexpr bool isControlEvent(ExecutionStatus status) noexcept
{
    const auto val = static_cast<uint16_t>(status);
    return val >= 0x2000 && val < 0x3000;
}

constexpr bool isError(ExecutionStatus status) noexcept
{
    return static_cast<uint16_t>(status) >= static_cast<uint16_t>(ExecutionStatus::Error);
}

constexpr bool isSuccess(ExecutionStatus status) noexcept
{
    return !isError(status);
}

constexpr bool isBranch(ExecutionStatus status) noexcept
{
    return status == ExecutionStatus::BranchTaken;
}

constexpr bool isJump(ExecutionStatus status) noexcept
{
    return status == ExecutionStatus::JumpTaken;
}

} // namespace Core