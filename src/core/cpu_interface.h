#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include "primitives.h"

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

class ICpu
{
public:
    virtual ~ICpu() = default;

    virtual ExecutionStatus step() = 0;
    virtual void reset() = 0;

    virtual uint64_t getPc() const = 0;
    virtual void setPc(uint64_t pc) = 0;

    virtual uint32_t getRegisterCount() const = 0;
    virtual uint64_t getRegister(uint32_t index) const = 0;
    virtual void setRegister(uint32_t index, uint64_t value) = 0;
    virtual std::string_view getRegisterName(uint32_t index) const = 0;
    virtual const CpuInfo& getCpuInfo() const = 0;

    // string representation of the last instruction executed
    virtual std::string disassembleCurrent() const = 0;
};

} // namespace Core
