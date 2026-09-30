#pragma once
#include <cstdint>
#include <array>
#include "instruction.h"

namespace RV32
{

class Decoder
{
private:
    using OpcodeDecoderFunc = DecodedInstruction (Decoder::*)(uint32_t) const;

    std::array<OpcodeDecoderFunc, 128> m_instTypeDecoderLUT;

    std::array<InstructionId, 8> m_immGroupLUT;
    // (func7[5] | func3[2:0]) -> INSTR
    std::array<InstructionId, 16> m_opGroupLUT;
    std::array<InstructionId, 8> m_branchGroupLUT;
    std::array<InstructionId, 8> m_loadGroupLUT;
    std::array<InstructionId, 8> m_storeGroupLUT;

    DecodedInstruction decodeOP_IMM(uint32_t instr) const;
    DecodedInstruction decodeLUI(uint32_t instr) const;
    DecodedInstruction decodeAUIPC(uint32_t instr) const;
    DecodedInstruction decodeOP(uint32_t instr) const;
    DecodedInstruction decodeJAL(uint32_t instr) const;
    DecodedInstruction decodeJALR(uint32_t instr) const;
    DecodedInstruction decodeBRANCH(uint32_t instr) const;
    DecodedInstruction decodeLOAD(uint32_t instr) const;
    DecodedInstruction decodeSTORE(uint32_t instr) const;
    DecodedInstruction decodeMISC_MEM(uint32_t instr) const;
    DecodedInstruction decodeSYSTEM(uint32_t instr) const;

    int32_t decodeImmFieldForTypeI(uint32_t instr) const;
    int32_t decodeImmFieldForTypeU(uint32_t instr) const;
    int32_t decodeImmFieldForTypeJ(uint32_t instr) const;
    int32_t decodeImmFieldForTypeB(uint32_t instr) const;
    int32_t decodeImmFieldForTypeS(uint32_t instr) const;

    uint8_t decodeRdField(uint32_t instr) const;
    uint8_t decodeRs1Field(uint32_t instr) const;
    uint8_t decodeRs2Field(uint32_t instr) const;
    uint8_t decodeFunct3Field(uint32_t instr) const;
    uint8_t decodeFunct7Field(uint32_t instr) const;

    void mapInstTypeDecoderLUT();
    void mapImmGroupLUT();
    void mapOpGroupLUT();
    void mapBranchGroupLUT();
    void mapLoadGroupLUT();
    void mapStoreGroupLUT();
public:
    Decoder();
    static bool checkInstruction(InstructionId instrId);
    DecodedInstruction decode(uint32_t instr) const;
};

} // namespace RV32
