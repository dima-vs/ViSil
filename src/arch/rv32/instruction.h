#pragma once
#include <cstdint>

namespace RV32
{

enum class InstructionId
{
    Unknown, UnknownIllegal,
    ADD, SUB, XOR, OR, AND, SLL, SRL, SRA, SLT, SLTU,
    ADDI, XORI, ORI, ANDI, SLLI, SRLI, SRAI, SLTI, SLTIU,
    LB, LH, LW, LBU, LHU,
    SB, SH, SW,
    BEQ, BNE, BLT, BGE, BLTU, BGEU,
    JAL, JALR,
    LUI, AUIPC,
    ECALL, EBREAK,
    FENCE, FENCE_TSO, PAUSE,
    Count
};

enum class Format { R, I, S, B, U, J, Unknown };

struct DecodedInstruction
{
    InstructionId id = InstructionId::Unknown;
    Format format = Format::Unknown;

    uint8_t rd = 0;
    uint8_t rs1 = 0;
    uint8_t rs2 = 0;

    int32_t imm = 0;
};

} // namespace RV32
