#include "execution_unit.h"

namespace RV32
{

ExecutionUnit::ExecutionUnit(
    RegisterFile &regFile,
    Core::IBusDevice &busDevice) :
    m_regFile(regFile),
    m_busDevice(busDevice)
{
    mapExecHandlerLUT();
}

ExecStatus ExecutionUnit::exec(const DecodedInstruction& decInstr)
{
    auto execHandler = m_execHandlerLUT[decInstr.id];
    if (execHandler == nullptr)
        return ExecStatus::IllegalInstruction;

    return (this->*execHandler)(decInstr);
}

void ExecutionUnit::mapExecHandlerLUT()
{
    m_execHandlerLUT.data.fill(nullptr);

    m_execHandlerLUT[InstructionId::ADD] = &ExecutionUnit::Add;
    m_execHandlerLUT[InstructionId::SUB] = &ExecutionUnit::Sub;
    m_execHandlerLUT[InstructionId::XOR] = &ExecutionUnit::Xor;
    m_execHandlerLUT[InstructionId::OR] = &ExecutionUnit::Or;
    m_execHandlerLUT[InstructionId::AND] = &ExecutionUnit::And;
    m_execHandlerLUT[InstructionId::SLL] = &ExecutionUnit::Sll;
    m_execHandlerLUT[InstructionId::SRL] = &ExecutionUnit::Srl;
    m_execHandlerLUT[InstructionId::SRA] = &ExecutionUnit::Sra;
    m_execHandlerLUT[InstructionId::SLT] = &ExecutionUnit::Slt;
    m_execHandlerLUT[InstructionId::SLTU] = &ExecutionUnit::Sltu;

    m_execHandlerLUT[InstructionId::ADDI] = &ExecutionUnit::Addi;
    m_execHandlerLUT[InstructionId::XORI] = &ExecutionUnit::Xori;
    m_execHandlerLUT[InstructionId::ORI] = &ExecutionUnit::Ori;
    m_execHandlerLUT[InstructionId::ANDI] = &ExecutionUnit::Andi;
    m_execHandlerLUT[InstructionId::SLLI] = &ExecutionUnit::Slli;
    m_execHandlerLUT[InstructionId::SRLI] = &ExecutionUnit::Srli;
    m_execHandlerLUT[InstructionId::SRAI] = &ExecutionUnit::Srai;
    m_execHandlerLUT[InstructionId::SLTI] = &ExecutionUnit::Slti;
    m_execHandlerLUT[InstructionId::SLTIU] = &ExecutionUnit::Sltiu;

    m_execHandlerLUT[InstructionId::LB] = &ExecutionUnit::Lb;
    m_execHandlerLUT[InstructionId::LH] = &ExecutionUnit::Lh;
    m_execHandlerLUT[InstructionId::LW] = &ExecutionUnit::Lw;
    m_execHandlerLUT[InstructionId::LBU] = &ExecutionUnit::Lbu;
    m_execHandlerLUT[InstructionId::LHU] = &ExecutionUnit::Lhu;

    m_execHandlerLUT[InstructionId::SB] = &ExecutionUnit::Sb;
    m_execHandlerLUT[InstructionId::SH] = &ExecutionUnit::Sh;
    m_execHandlerLUT[InstructionId::SW] = &ExecutionUnit::Sw;

    m_execHandlerLUT[InstructionId::BEQ] = &ExecutionUnit::Beq;
    m_execHandlerLUT[InstructionId::BNE] = &ExecutionUnit::Bne;
    m_execHandlerLUT[InstructionId::BLT] = &ExecutionUnit::Blt;
    m_execHandlerLUT[InstructionId::BGE] = &ExecutionUnit::Bge;
    m_execHandlerLUT[InstructionId::BLTU] = &ExecutionUnit::Bltu;
    m_execHandlerLUT[InstructionId::BGEU] = &ExecutionUnit::Bgeu;

    m_execHandlerLUT[InstructionId::JAL] = &ExecutionUnit::Jal;
    m_execHandlerLUT[InstructionId::JALR] = &ExecutionUnit::Jalr;

    m_execHandlerLUT[InstructionId::LUI] = &ExecutionUnit::Lui;
    m_execHandlerLUT[InstructionId::AUIPC] = &ExecutionUnit::Auipc;

    m_execHandlerLUT[InstructionId::ECALL] = &ExecutionUnit::Ecall;
    m_execHandlerLUT[InstructionId::EBREAK] = &ExecutionUnit::Ebreak;

    m_execHandlerLUT[InstructionId::FENCE] = &ExecutionUnit::Nop;
    m_execHandlerLUT[InstructionId::FENCE_TSO] = &ExecutionUnit::Nop;
    m_execHandlerLUT[InstructionId::PAUSE] = &ExecutionUnit::Nop;

}

ExecStatus ExecutionUnit::Add(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t rs2V = m_regFile.readGPR(decInstr.rs2);
    m_regFile.writeGPR(decInstr.rd, rs1V + rs2V);
    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Sub(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t rs2V = m_regFile.readGPR(decInstr.rs2);
    m_regFile.writeGPR(decInstr.rd, rs1V - rs2V);
    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Xor(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t rs2V = m_regFile.readGPR(decInstr.rs2);
    m_regFile.writeGPR(decInstr.rd, rs1V ^ rs2V);
    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Or(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t rs2V = m_regFile.readGPR(decInstr.rs2);
    m_regFile.writeGPR(decInstr.rd, rs1V | rs2V);
    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::And(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t rs2V = m_regFile.readGPR(decInstr.rs2);
    m_regFile.writeGPR(decInstr.rd, rs1V & rs2V);
    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Sll(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t shift = m_regFile.readGPR(decInstr.rs2) & 0x1F;
    m_regFile.writeGPR(decInstr.rd, rs1V << shift);
    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Srl(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t shift = m_regFile.readGPR(decInstr.rs2) & 0x1F;
    m_regFile.writeGPR(decInstr.rd, rs1V >> shift);
    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Sra(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t shift = m_regFile.readGPR(decInstr.rs2) & 0x1F;
    m_regFile.writeGPR(decInstr.rd, static_cast<int32_t>(rs1V) >> shift);
    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Slt(const DecodedInstruction& decInstr)
{
    int32_t rs1V = static_cast<int32_t>(m_regFile.readGPR(decInstr.rs1));
    int32_t rs2V = static_cast<int32_t>(m_regFile.readGPR(decInstr.rs2));
    uint32_t sltRes = (rs1V < rs2V) ? 1 : 0;
    m_regFile.writeGPR(decInstr.rd, sltRes);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Sltu(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t rs2V = m_regFile.readGPR(decInstr.rs2);
    uint32_t sltRes = (rs1V < rs2V) ? 1 : 0;
    m_regFile.writeGPR(decInstr.rd, sltRes);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Addi(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);
    m_regFile.writeGPR(decInstr.rd, rs1V + imm);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Xori(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);
    m_regFile.writeGPR(decInstr.rd, rs1V ^ imm);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Ori(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);
    m_regFile.writeGPR(decInstr.rd, rs1V | imm);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Andi(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);
    m_regFile.writeGPR(decInstr.rd, rs1V & imm);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Slli(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t shift = static_cast<uint32_t>(decInstr.imm) & 0x1F;
    m_regFile.writeGPR(decInstr.rd, rs1V << shift);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Srli(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t shift = static_cast<uint32_t>(decInstr.imm) & 0x1F;
    m_regFile.writeGPR(decInstr.rd, rs1V >> shift);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Srai(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t shift = static_cast<uint32_t>(decInstr.imm) & 0x1F;
    m_regFile.writeGPR(decInstr.rd, static_cast<int32_t>(rs1V) >> shift);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Slti(const DecodedInstruction& decInstr)
{
    int32_t rs1V = static_cast<int32_t>(m_regFile.readGPR(decInstr.rs1));
    int32_t imm = decInstr.imm;
    uint32_t sltRes = (rs1V < imm) ? 1 : 0;
    m_regFile.writeGPR(decInstr.rd, sltRes);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Sltiu(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);
    uint32_t sltRes = (rs1V < imm) ? 1 : 0;
    m_regFile.writeGPR(decInstr.rd, sltRes);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Lb(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);
    uint32_t addr = rs1V + imm;

    uint8_t readVal;
    Core::BusStatus busStatus = m_busDevice.read8(addr, readVal);
    if (busStatus != Core::BusStatus::Ok)
        return ExecStatus::MemoryAccessFault;

    int32_t readValExt = (static_cast<int32_t>(readVal) << 24) >> 24;
    m_regFile.writeGPR(decInstr.rd, static_cast<uint32_t>(readValExt));

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Lh(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);
    uint32_t addr = rs1V + imm;

    uint16_t readVal;
    Core::BusStatus busStatus = m_busDevice.read16(addr, readVal);
    if (busStatus != Core::BusStatus::Ok)
        return ExecStatus::MemoryAccessFault;

    int32_t readValExt = (static_cast<int32_t>(readVal) << 16) >> 16;
    m_regFile.writeGPR(decInstr.rd, static_cast<uint32_t>(readValExt));

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Lw(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);
    uint32_t addr = rs1V + imm;

    uint32_t readVal;
    Core::BusStatus busStatus = m_busDevice.read32(addr, readVal);
    if (busStatus != Core::BusStatus::Ok)
        return ExecStatus::MemoryAccessFault;

    m_regFile.writeGPR(decInstr.rd, readVal);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Lbu(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);
    uint32_t addr = rs1V + imm;

    uint8_t readVal;
    Core::BusStatus busStatus = m_busDevice.read8(addr, readVal);
    if (busStatus != Core::BusStatus::Ok)
        return ExecStatus::MemoryAccessFault;

    uint32_t readValExt = static_cast<uint32_t>(readVal);
    m_regFile.writeGPR(decInstr.rd, readValExt);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Lhu(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);
    uint32_t addr = rs1V + imm;

    uint16_t readVal;
    Core::BusStatus busStatus = m_busDevice.read16(addr, readVal);
    if (busStatus != Core::BusStatus::Ok)
        return ExecStatus::MemoryAccessFault;

    uint32_t readValExt = static_cast<uint32_t>(readVal);
    m_regFile.writeGPR(decInstr.rd, readValExt);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Sb(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t rs2V = m_regFile.readGPR(decInstr.rs2);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);

    uint32_t addr = rs1V + imm;
    uint8_t byteToWrite = static_cast<uint8_t>(rs2V);

    Core::BusStatus busStatus = m_busDevice.write8(addr, byteToWrite);
    if (busStatus != Core::BusStatus::Ok)
        return ExecStatus::MemoryAccessFault;

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Sh(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t rs2V = m_regFile.readGPR(decInstr.rs2);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);

    uint32_t addr = rs1V + imm;
    uint16_t halfwToWrite = static_cast<uint16_t>(rs2V);

    Core::BusStatus busStatus = m_busDevice.write16(addr, halfwToWrite);
    if (busStatus != Core::BusStatus::Ok)
        return ExecStatus::MemoryAccessFault;

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Sw(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t rs2V = m_regFile.readGPR(decInstr.rs2);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);

    uint32_t addr = rs1V + imm;
    uint32_t wordToWrite = static_cast<uint32_t>(rs2V);

    Core::BusStatus busStatus = m_busDevice.write32(addr, wordToWrite);
    if (busStatus != Core::BusStatus::Ok)
        return ExecStatus::MemoryAccessFault;

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Beq(const DecodedInstruction& decInstr)
{
    int32_t rs1V = static_cast<int32_t>(m_regFile.readGPR(decInstr.rs1));
    int32_t rs2V = static_cast<int32_t>(m_regFile.readGPR(decInstr.rs2));
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);

    uint32_t currentPc = m_regFile.getPC();
    uint32_t newPc = currentPc + imm;

    if (rs1V == rs2V)
    {
        if (newPc % 4 != 0)
            return ExecStatus::MisalignedAccess;

        m_regFile.setPC(newPc);
        return ExecStatus::BranchTaken;
    }

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Bne(const DecodedInstruction& decInstr)
{
    int32_t rs1V = static_cast<int32_t>(m_regFile.readGPR(decInstr.rs1));
    int32_t rs2V = static_cast<int32_t>(m_regFile.readGPR(decInstr.rs2));
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);

    uint32_t currentPc = m_regFile.getPC();
    uint32_t newPc = currentPc + imm;

    if (rs1V != rs2V)
    {
        if (newPc % 4 != 0)
            return ExecStatus::MisalignedAccess;

        m_regFile.setPC(newPc);
        return ExecStatus::BranchTaken;
    }

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Blt(const DecodedInstruction& decInstr)
{
    int32_t rs1V = static_cast<int32_t>(m_regFile.readGPR(decInstr.rs1));
    int32_t rs2V = static_cast<int32_t>(m_regFile.readGPR(decInstr.rs2));
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);

    uint32_t currentPc = m_regFile.getPC();
    uint32_t newPc = currentPc + imm;

    if (rs1V < rs2V)
    {
        if (newPc % 4 != 0)
            return ExecStatus::MisalignedAccess;

        m_regFile.setPC(newPc);
        return ExecStatus::BranchTaken;
    }

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Bge(const DecodedInstruction& decInstr)
{
    int32_t rs1V = static_cast<int32_t>(m_regFile.readGPR(decInstr.rs1));
    int32_t rs2V = static_cast<int32_t>(m_regFile.readGPR(decInstr.rs2));
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);

    uint32_t currentPc = m_regFile.getPC();
    uint32_t newPc = currentPc + imm;

    if (rs1V >= rs2V)
    {
        if (newPc % 4 != 0)
            return ExecStatus::MisalignedAccess;

        m_regFile.setPC(newPc);
        return ExecStatus::BranchTaken;
    }

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Bltu(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t rs2V = m_regFile.readGPR(decInstr.rs2);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);

    uint32_t currentPc = m_regFile.getPC();
    uint32_t newPc = currentPc + imm;

    if (rs1V < rs2V)
    {
        if (newPc % 4 != 0)
            return ExecStatus::MisalignedAccess;

        m_regFile.setPC(newPc);
        return ExecStatus::BranchTaken;
    }

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Bgeu(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t rs2V = m_regFile.readGPR(decInstr.rs2);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);

    uint32_t currentPc = m_regFile.getPC();
    uint32_t newPc = currentPc + imm;

    if (rs1V >= rs2V)
    {
        if (newPc % 4 != 0)
            return ExecStatus::MisalignedAccess;

        m_regFile.setPC(newPc);
        return ExecStatus::BranchTaken;
    }

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Jal(const DecodedInstruction& decInstr)
{
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);
    uint32_t currentPc = m_regFile.getPC();
    uint32_t newPc = currentPc + imm;

    if (newPc % 4 != 0)
        return ExecStatus::MisalignedAccess;

    m_regFile.writeGPR(decInstr.rd, currentPc + 4);
    m_regFile.setPC(newPc);

    return ExecStatus::JumpTaken;
}

ExecStatus ExecutionUnit::Jalr(const DecodedInstruction& decInstr)
{
    uint32_t rs1V = m_regFile.readGPR(decInstr.rs1);
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);
    uint32_t currentPc = m_regFile.getPC();
    uint32_t newPc = (rs1V + imm) & ~0x1;

    if (newPc % 4 != 0)
        return ExecStatus::MisalignedAccess;

    m_regFile.writeGPR(decInstr.rd, currentPc + 4);
    m_regFile.setPC(newPc);

    return ExecStatus::JumpTaken;
}

ExecStatus ExecutionUnit::Lui(const DecodedInstruction& decInstr)
{
    // imm is already aligned so that the lowest 12 bits are zero
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);
    m_regFile.writeGPR(decInstr.rd, imm);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Auipc(const DecodedInstruction& decInstr)
{
    // imm is already aligned so that the lowest 12 bits are zero
    uint32_t imm = static_cast<uint32_t>(decInstr.imm);
    uint32_t currentPc = m_regFile.getPC();
    m_regFile.writeGPR(decInstr.rd, currentPc + imm);

    return ExecStatus::Ok;
}

ExecStatus ExecutionUnit::Ecall(const DecodedInstruction& decInstr)
{
    return ExecStatus::EnvCall;
}

ExecStatus ExecutionUnit::Ebreak(const DecodedInstruction& decInstr)
{
    return ExecStatus::BreakpointHit;
}

ExecStatus ExecutionUnit::Nop(const DecodedInstruction& decInstr)
{
    return ExecStatus::Ok;
}

} // namespace RV32
