#include "decoder.h"
#include <cassert>

namespace RV32
{

Decoder::Decoder()
{
    mapInstTypeDecoderLUT();
    mapImmGroupLUT();
    mapOpGroupLUT();
    mapBranchGroupLUT();
    mapLoadGroupLUT();
}

bool Decoder::checkInstruction(InstructionId instrId)
{
    if (instrId >= InstructionId::Count)
        return false;

    return (instrId != InstructionId::Unknown) &&
           (instrId != InstructionId::UnknownIllegal);
}

void Decoder::mapInstTypeDecoderLUT()
{
    m_instTypeDecoderLUT.fill(nullptr);

    m_instTypeDecoderLUT[0b011'0111] = &Decoder::decodeLUI;
    m_instTypeDecoderLUT[0b001'0111] = &Decoder::decodeAUIPC;
    m_instTypeDecoderLUT[0b110'1111] = &Decoder::decodeJAL;
    m_instTypeDecoderLUT[0b110'0111] = &Decoder::decodeJALR;
    m_instTypeDecoderLUT[0b110'0011] = &Decoder::decodeBRANCH;
    m_instTypeDecoderLUT[0b000'0011] = &Decoder::decodeLOAD;
    m_instTypeDecoderLUT[0b010'0011] = &Decoder::decodeSTORE;
    m_instTypeDecoderLUT[0b001'0011] = &Decoder::decodeOP_IMM;
    m_instTypeDecoderLUT[0b011'0011] = &Decoder::decodeOP;
    m_instTypeDecoderLUT[0b000'1111] = &Decoder::decodeMISC_MEM;
    m_instTypeDecoderLUT[0b111'0011] = &Decoder::decodeSYSTEM;
}

void Decoder::mapImmGroupLUT()
{
    m_immGroupLUT.fill(InstructionId::UnknownIllegal);

    m_immGroupLUT[0b000] = InstructionId::ADDI;
    m_immGroupLUT[0b010] = InstructionId::SLTI;
    m_immGroupLUT[0b011] = InstructionId::SLTIU;
    m_immGroupLUT[0b100] = InstructionId::XORI;
    m_immGroupLUT[0b110] = InstructionId::ORI;
    m_immGroupLUT[0b111] = InstructionId::ANDI;
    m_immGroupLUT[0b001] = InstructionId::Unknown;
    m_immGroupLUT[0b101] = InstructionId::Unknown;
}

void Decoder::mapOpGroupLUT()
{
    m_opGroupLUT.fill(InstructionId::UnknownIllegal);

    // addr = (func7[5] | func3[2:0])
    m_opGroupLUT[0b0'000] = InstructionId::ADD;
    m_opGroupLUT[0b1'000] = InstructionId::SUB;
    m_opGroupLUT[0b0'001] = InstructionId::SLL;
    m_opGroupLUT[0b0'010] = InstructionId::SLT;
    m_opGroupLUT[0b0'011] = InstructionId::SLTU;
    m_opGroupLUT[0b0'100] = InstructionId::XOR;
    m_opGroupLUT[0b0'101] = InstructionId::SRL;
    m_opGroupLUT[0b1'101] = InstructionId::SRA;
    m_opGroupLUT[0b0'110] = InstructionId::OR;
    m_opGroupLUT[0b0'111] = InstructionId::AND;
}

void Decoder::mapBranchGroupLUT()
{
    m_branchGroupLUT.fill(InstructionId::UnknownIllegal);

    m_branchGroupLUT[0b000] = InstructionId::BEQ;
    m_branchGroupLUT[0b001] = InstructionId::BNE;
    m_branchGroupLUT[0b100] = InstructionId::BLT;
    m_branchGroupLUT[0b101] = InstructionId::BGE;
    m_branchGroupLUT[0b110] = InstructionId::BLTU;
    m_branchGroupLUT[0b111] = InstructionId::BGEU;
}

void Decoder::mapLoadGroupLUT()
{
    m_loadGroupLUT.fill(InstructionId::UnknownIllegal);

    m_loadGroupLUT[0b000] = InstructionId::LB;
    m_loadGroupLUT[0b001] = InstructionId::LH;
    m_loadGroupLUT[0b010] = InstructionId::LW;
    m_loadGroupLUT[0b100] = InstructionId::LBU;
    m_loadGroupLUT[0b101] = InstructionId::LHU;
}

int32_t Decoder::decodeImmFieldForTypeI(uint32_t instr) const
{
    return static_cast<int32_t>(instr) >> 20;
}

int32_t Decoder::decodeImmFieldForTypeU(uint32_t instr) const
{
    return static_cast<int32_t>(instr) & 0xFFFFF000;
}

int32_t Decoder::decodeImmFieldForTypeJ(uint32_t instr) const
{
    return (
        (static_cast<int32_t>(instr) & 0x80000000) |
        ((static_cast<int32_t>(instr) & 0x000FF000) << 11) |
        ((static_cast<int32_t>(instr) & 0x00100000) << 2) |
        ((static_cast<int32_t>(instr) & 0x7FE00000) >> 9)
            ) >> 11;
}

int32_t Decoder::decodeImmFieldForTypeB(uint32_t instr) const
{
    return (
        (static_cast<int32_t>(instr) & 0x80000000) |
        ((static_cast<int32_t>(instr) & 0x00000080) << 23) |
        ((static_cast<int32_t>(instr) & 0x7E000000) >> 1) |
        ((static_cast<int32_t>(instr) & 0x00000F00) << 12)
       ) >> 19;
}

uint8_t Decoder::decodeRdField(uint32_t instr) const
{
    return static_cast<uint8_t>((instr >> 7) & 0x1F);
}

uint8_t Decoder::decodeRs1Field(uint32_t instr) const
{
    return static_cast<uint8_t>((instr >> 15) & 0x1F);
}

uint8_t Decoder::decodeRs2Field(uint32_t instr) const
{
    return static_cast<uint8_t>((instr >> 20) & 0x1F);
}

uint8_t Decoder::decodeFunct3Field(uint32_t instr) const
{
    return static_cast<uint8_t>((instr >> 12) & 0x7);
}

uint8_t Decoder::decodeFunct7Field(uint32_t instr) const
{
    return static_cast<uint8_t>((instr >> 25) & 0x7F);
}

DecodedInstruction Decoder::decodeOP_IMM(uint32_t instr) const
{
    uint8_t funct3 = decodeFunct3Field(instr);
    uint8_t funct7 = decodeFunct7Field(instr);
    InstructionId instrId = m_immGroupLUT[funct3];
    DecodedInstruction dec;

    dec.id = instrId;
    dec.format = Format::I;
    dec.rd = decodeRdField(instr);
    dec.rs1 = decodeRs1Field(instr);
    dec.imm = decodeImmFieldForTypeI(instr);

    switch (instrId)
    {
    case InstructionId::UnknownIllegal:
        dec.format = Format::Unknown;
        dec.id = InstructionId::Unknown;
        break;
    case InstructionId::Unknown: // shift instructions
        dec.imm &= 0x1F;

        if (funct3 == 0b001 && funct7 == 0b000'0000)
        {
            dec.id = InstructionId::SLLI;
        } else if (funct3 == 0b101 && funct7 == 0b000'0000)
        {
            dec.id = InstructionId::SRLI;
        } else if (funct3 == 0b101 && funct7 == 0b010'0000)
        {
            dec.id = InstructionId::SRAI;
        } else
        {
            dec.id = InstructionId::Unknown;
            dec.format = Format::Unknown;
        }
        break;
    }

    return dec;
}

DecodedInstruction Decoder::decodeLUI(uint32_t instr) const
{
    DecodedInstruction dec;

    dec.id = InstructionId::LUI;
    dec.format = Format::U;
    dec.imm = decodeImmFieldForTypeU(instr);
    dec.rd = decodeRdField(instr);

    return dec;
}

DecodedInstruction Decoder::decodeAUIPC(uint32_t instr) const
{
    DecodedInstruction dec;

    dec.id = InstructionId::AUIPC;
    dec.format = Format::U;
    dec.imm = decodeImmFieldForTypeU(instr);
    dec.rd = decodeRdField(instr);

    return dec;
}

DecodedInstruction Decoder::decodeOP(uint32_t instr) const
{
    uint8_t funct3 = decodeFunct3Field(instr);
    uint8_t funct7 = decodeFunct7Field(instr);
    uint8_t lutAddr = ((funct7 & static_cast<uint8_t>(0x20)) >> 2) |
                      funct3;
    // all func7 bits except func7[5] always have to be zero
    uint8_t func7_OtherBits = (funct7 & static_cast<uint8_t>(0x5F));
    InstructionId instrId = m_opGroupLUT[lutAddr];

    DecodedInstruction dec;
    dec.id = instrId;
    dec.format = Format::R;
    dec.rd = decodeRdField(instr);
    dec.rs1 = decodeRs1Field(instr);
    dec.rs2 = decodeRs2Field(instr);

    if ((instrId == InstructionId::UnknownIllegal) ||
        (func7_OtherBits != 0x00))
    {
        dec.id = InstructionId::Unknown;
        dec.format = Format::Unknown;
    }

    return dec;
}

DecodedInstruction Decoder::decodeJAL(uint32_t instr) const
{
    DecodedInstruction dec;

    dec.id = InstructionId::JAL;
    dec.format = Format::J;
    dec.imm = decodeImmFieldForTypeJ(instr);
    dec.rd = decodeRdField(instr);

    return dec;
}

DecodedInstruction Decoder::decodeJALR(uint32_t instr) const
{
    DecodedInstruction dec;

    uint8_t funct3 = decodeFunct3Field(instr);
    if (funct3 != 0b000)
    {
        dec.id = InstructionId::Unknown;
        dec.format = Format::Unknown;
        return dec;
    }

    dec.id = InstructionId::JALR;
    dec.format = Format::I;
    dec.rd = decodeRdField(instr);
    dec.rs1 = decodeRs1Field(instr);
    dec.imm = decodeImmFieldForTypeI(instr);

    return dec;
}

DecodedInstruction Decoder::decodeBRANCH(uint32_t instr) const
{
    uint8_t funct3 = decodeFunct3Field(instr);
    InstructionId instrId = m_branchGroupLUT[funct3];

    DecodedInstruction dec;

    dec.id = instrId;
    dec.format = Format::B;
    dec.rs1 = decodeRs1Field(instr);
    dec.rs2 = decodeRs2Field(instr);
    dec.imm = decodeImmFieldForTypeB(instr);

    if (instrId == InstructionId::UnknownIllegal)
    {
        dec.id = InstructionId::Unknown;
        dec.format = Format::Unknown;
    }

    return dec;
}

DecodedInstruction Decoder::decodeLOAD(uint32_t instr) const
{
    uint8_t funct3 = decodeFunct3Field(instr);
    InstructionId instrId = m_loadGroupLUT[funct3];
    DecodedInstruction dec;

    dec.id = instrId;
    dec.format = Format::I;
    dec.rd = decodeRdField(instr);
    dec.rs1 = decodeRs1Field(instr);
    dec.imm = decodeImmFieldForTypeI(instr);

    if (instrId == InstructionId::UnknownIllegal)
    {
        dec.id = InstructionId::Unknown;
        dec.format = Format::Unknown;
    }

    return dec;
}

DecodedInstruction Decoder::decodeSTORE(uint32_t instr) const
{
    return DecodedInstruction();
}

DecodedInstruction Decoder::decodeMISC_MEM(uint32_t instr) const
{
    return DecodedInstruction();
}

DecodedInstruction Decoder::decodeSYSTEM(uint32_t instr) const
{
    return DecodedInstruction();
}

} // namespace RV32
