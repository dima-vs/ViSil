#pragma once
#include <cstdint>
#include <string_view>
#include <string>
#include "cpu_interface.h"
#include "register_file.h"
#include "bus_device_interface.h"
#include "decoder.h"
#include "execution_unit.h"

namespace RV32
{

class Cpu : public Core::ICpu
{
private:
    static constexpr uint8_t PC_STEP_SIZE = 4;

    Core::CpuInfo m_cpuInfo;
    RegisterFile &m_regFile;
    Core::IBusDevice &m_busDevice;
    Decoder m_decoder;
    ExecutionUnit m_execUnit;

    Core::BusStatus fetch(uint32_t &outInstr) const;
    void advancePcIfNeeded(Core::ExecutionStatus execStatus);
    void initCpuInfo();
public:
    Cpu(RegisterFile &regFile, Core::IBusDevice &busDevice);

    Core::ExecutionStatus step() override;
    void reset() override;

    uint64_t getPc() const override;
    void setPc(uint64_t pc) override;

    uint32_t getRegisterCount() const override;
    uint64_t getRegister(uint32_t index) const override;
    void setRegister(uint32_t index, uint64_t value) override;
    std::string_view getRegisterName(uint32_t index) const override;
    const Core::CpuInfo& getCpuInfo() const override;

    std::string disassembleCurrent() const override;
};

} // namespace RV32
