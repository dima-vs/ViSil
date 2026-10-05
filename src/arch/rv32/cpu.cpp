#include "cpu.h"

namespace RV32
{

Cpu::Cpu(RegisterFile &regFile, Core::IBusDevice &busDevice) :
    m_regFile(regFile), m_busDevice(busDevice),
    m_execUnit(regFile, busDevice)
{
    initCpuInfo();
}

Core::BusStatus Cpu::fetch(uint32_t &outInstr) const
{
    return m_busDevice.read32(m_regFile.getPC(), outInstr);
}

Core::ExecutionStatus Cpu::step()
{
    uint32_t rawInstr;
    Core::BusStatus busStatus = fetch(rawInstr);

    Core::ExecutionStatus execStatus = Core::mapBusStatusToExecStatus(busStatus);
    if (execStatus != Core::ExecutionStatus::Ok)
        return execStatus;

    DecodedInstruction decInstr = m_decoder.decode(rawInstr);
    if (!Decoder::checkInstruction(decInstr.id))
        return Core::ExecutionStatus::IllegalInstruction;

    execStatus = m_execUnit.exec(decInstr);
    advancePcIfNeeded(execStatus);
    return execStatus;
}

void Cpu::advancePcIfNeeded(Core::ExecutionStatus execStatus)
{
    if (!Core::isPcChanged(execStatus))
        m_regFile.advancePC(Cpu::PC_STEP_SIZE);
}

void Cpu::reset()
{
    m_regFile.resetAllGPR();
    m_regFile.setPC(0);
}

uint64_t Cpu::getPc() const
{
    return m_regFile.getPC();
}

void Cpu::setPc(uint64_t pc)
{
    m_regFile.setPC(static_cast<uint32_t>(pc));
}

uint32_t Cpu::getRegisterCount() const
{
    return RegisterFile::GPR_COUNT;
}

uint64_t Cpu::getRegister(uint32_t index) const
{
    return m_regFile.readGPR(index);
}

void Cpu::setRegister(uint32_t index, uint64_t value)
{
    m_regFile.writeGPR(index, static_cast<uint32_t>(value));
}

std::string_view Cpu::getRegisterName(uint32_t index) const
{
    return RegisterFile::getAbiName(index);
}

void Cpu::initCpuInfo()
{
    m_cpuInfo.archFamily = Core::ArchFamily::Risc;
    m_cpuInfo.archName = "RV32I";
    m_cpuInfo.registerCount = getRegisterCount();
    m_cpuInfo.supportedInstrCount = static_cast<uint32_t>(InstructionId::Count);
    m_cpuInfo.wordWidth = Core::BitWidth::Bits32;
}

const Core::CpuInfo& Cpu::getCpuInfo() const
{
    return m_cpuInfo;
}

std::string Cpu::disassembleCurrent() const
{
    return "<disassembly unavailable>";
}

} // namespace RV32
