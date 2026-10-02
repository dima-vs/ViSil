#pragma once
#include <cstdint>
#include <array>
#include "register_file.h"
#include "instruction.h"
#include "types.h"
#include "bus_device_interface.h"

namespace RV32
{

using ExecStatus = Core::ExecutionStatus;

class ExecutionUnit
{
private:
    using ExecHandler = ExecStatus (ExecutionUnit::*)(const DecodedInstruction&);
    struct InstrIdMap
    {
        std::array<ExecHandler, static_cast<size_t>(InstructionId::Count)> data;

        constexpr ExecHandler& operator[] (InstructionId key)
        {
            return data[static_cast<size_t>(key)];
        }

        constexpr const ExecHandler& operator[] (InstructionId key) const
        {
            return data[static_cast<size_t>(key)];
        }
    };

    InstrIdMap m_execHandlerLUT;

    RegisterFile &m_regFile;
    Core::IBusDevice &m_busDevice;

    void mapExecHandlerLUT();

    // OP
    ExecStatus Add(const DecodedInstruction& decInstr);
    ExecStatus Sub(const DecodedInstruction& decInstr);
    ExecStatus Xor(const DecodedInstruction& decInstr);
    ExecStatus Or(const DecodedInstruction& decInstr);
    ExecStatus And(const DecodedInstruction& decInstr);
    ExecStatus Sll(const DecodedInstruction& decInstr);
    ExecStatus Srl(const DecodedInstruction& decInstr);
    ExecStatus Sra(const DecodedInstruction& decInstr);
    ExecStatus Slt(const DecodedInstruction& decInstr);
    ExecStatus Sltu(const DecodedInstruction& decInstr);

    // OP-IMM
    ExecStatus Addi(const DecodedInstruction& decInstr);
    ExecStatus Xori(const DecodedInstruction& decInstr);
    ExecStatus Ori(const DecodedInstruction& decInstr);
    ExecStatus Andi(const DecodedInstruction& decInstr);
    ExecStatus Slli(const DecodedInstruction& decInstr);
    ExecStatus Srli(const DecodedInstruction& decInstr);
    ExecStatus Srai(const DecodedInstruction& decInstr);
    ExecStatus Slti(const DecodedInstruction& decInstr);
    ExecStatus Sltiu(const DecodedInstruction& decInstr);

    // LOAD
    ExecStatus Lb(const DecodedInstruction& decInstr);
    ExecStatus Lh(const DecodedInstruction& decInstr);
    ExecStatus Lw(const DecodedInstruction& decInstr);
    ExecStatus Lbu(const DecodedInstruction& decInstr);
    ExecStatus Lhu(const DecodedInstruction& decInstr);

    // STORE
    ExecStatus Sb(const DecodedInstruction& decInstr);
    ExecStatus Sh(const DecodedInstruction& decInstr);
    ExecStatus Sw(const DecodedInstruction& decInstr);

    // BRANCH
    ExecStatus Beq(const DecodedInstruction& decInstr);
    ExecStatus Bne(const DecodedInstruction& decInstr);
    ExecStatus Blt(const DecodedInstruction& decInstr);
    ExecStatus Bge(const DecodedInstruction& decInstr);
    ExecStatus Bltu(const DecodedInstruction& decInstr);
    ExecStatus Bgeu(const DecodedInstruction& decInstr);

    // JUMP
    ExecStatus Jal(const DecodedInstruction& decInstr);
    ExecStatus Jalr(const DecodedInstruction& decInstr);

    ExecStatus Lui(const DecodedInstruction& decInstr);
    ExecStatus Auipc(const DecodedInstruction& decInstr);

    //SYSTEM
    ExecStatus Ecall(const DecodedInstruction& decInstr);
    ExecStatus Ebreak(const DecodedInstruction& decInstr);

    ExecStatus Nop(const DecodedInstruction& decInstr);
public:
    ExecutionUnit(RegisterFile &regFile, Core::IBusDevice &busDevice);

    ExecStatus exec(const DecodedInstruction& decInstr);
};

} // namespace RV32
