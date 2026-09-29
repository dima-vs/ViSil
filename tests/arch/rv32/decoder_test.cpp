#include <gtest/gtest.h>
#include <cstdint>
#include "decoder.h"

using namespace RV32;

class DecoderTest : public ::testing::Test
{
protected:
    uint32_t rawInstr = 0;
    Decoder decoder;

    void setOpcode(uint8_t opcode)
    {
        uint32_t shiftedVal = (opcode & 0x7F);
        rawInstr &= ~0x7F;
        rawInstr |= shiftedVal;
    }

    void setRd(uint8_t rd)
    {
        uint32_t shiftedVal = (rd & 0x1F) << 7;
        rawInstr &= ~0xF80;
        rawInstr |= shiftedVal;
    }

    void setFunct3(uint8_t funct3)
    {
        uint32_t shiftedVal = (funct3 & 0x7) << 12;
        rawInstr &= ~0x7000;
        rawInstr |= shiftedVal;
    }

    void setRs1(uint8_t rs1)
    {
        uint32_t shiftedVal = (rs1 & 0x1F) << 15;
        rawInstr &= ~0xF8000;
        rawInstr |= shiftedVal;
    }

    void setRs2(uint8_t rs2)
    {
        uint32_t shiftedVal = (rs2 & 0x1F) << 20;
        rawInstr &= ~0x1F00000;
        rawInstr |= shiftedVal;
    }

    void setFunct7(uint8_t funct7)
    {
        uint32_t shiftedVal = (funct7 & 0x7F) << 25;
        rawInstr &= ~0xFE000000;
        rawInstr |= shiftedVal;
    }

    // Format: imm[11:0] -> [31:20]
    void setImmI(uint32_t imm)
    {
        uint32_t shiftedImm = (imm & 0xFFF) << 20;
        uint32_t mask = 0xFFF00000;
        rawInstr &= ~mask;
        rawInstr |= shiftedImm;
    }

    // Format: imm[11:5] -> [31:25], imm[4:0] -> [11:7]
    void setImmS(uint32_t imm)
    {
        uint32_t imm4_0 = (imm & 0x1F) << 7;
        uint32_t imm11_5 = ((imm >> 5) & 0x7F) << 25;

        uint32_t mask = 0xFE000F80;
        rawInstr &= ~mask;
        rawInstr |= (imm4_0 | imm11_5);
    }

    // Format: imm[12] -> [31], imm[10:5] -> [30:25],
    //         imm[4:1] -> [11:8], imm[11] -> [7]
    // imm[0] is always 0 and is not written in instruction
    void setImmB(uint32_t imm)
    {
        uint32_t bit11 = ((imm >> 11) & 0x1) << 7;
        uint32_t bits4_1 = ((imm >> 1)  & 0xF) << 8;
        uint32_t bits10_5 = ((imm >> 5)  & 0x3F) << 25;
        uint32_t bit12 = ((imm >> 12) & 0x1) << 31;

        uint32_t mask = 0xFE000F80;
        rawInstr &= ~mask;
        rawInstr |= (bit11 | bits4_1 | bits10_5 | bit12);
    }

    // Format: imm[31:12] -> [31:12]
    // imm[11:0] must be zero and thus is ignored
    void setImmU(uint32_t imm)
    {
        uint32_t val = imm & 0xFFFFF000;
        uint32_t mask = 0xFFFFF000;
        rawInstr &= ~mask;
        rawInstr |= val;
    }

    // Format: imm[20] -> [31], imm[10:1] -> [30:21],
    //         imm[11] -> [20], imm[19:12] -> [19:12]
    // imm[0] is always 0 and is not written in instruction
    void setImmJ(uint32_t imm)
    {
        uint32_t bits19_12 = ((imm >> 12) & 0xFF) << 12;
        uint32_t bit11 = ((imm >> 11) & 0x1) << 20;
        uint32_t bits10_1 = ((imm >> 1) & 0x3FF) << 21;
        uint32_t bit20 = ((imm >> 20) & 0x1) << 31;

        uint32_t mask = 0xFFFFF000;
        rawInstr &= ~mask;
        rawInstr |= (bits19_12 | bit11 | bits10_1 | bit20);
    }
};

TEST_F(DecoderTest, DecodeTypeI_ADDI_NegativeImm)
{
    // asm: addi x5, x6, -1
    rawInstr = 0xFFF30293;
    DecodedInstruction decInst = decoder.decode(rawInstr);

    EXPECT_EQ(decInst.id, InstructionId::ADDI);
    EXPECT_EQ(decInst.format, Format::I);
    EXPECT_EQ(decInst.rd, 5);
    EXPECT_EQ(decInst.rs1, 6);
    EXPECT_EQ(decInst.imm, -1);
}

TEST_F(DecoderTest, DecodeTypeI_SRAI)
{
    // asm: srai x1, x2, 4
    rawInstr = 0x40415093;
    DecodedInstruction decInst = decoder.decode(rawInstr);

    EXPECT_EQ(decInst.id, InstructionId::SRAI);
    EXPECT_EQ(decInst.format, Format::I);
    EXPECT_EQ(decInst.rd, 1);
    EXPECT_EQ(decInst.rs1, 2);
    EXPECT_EQ(decInst.imm, 4);
}

TEST_F(DecoderTest, DecodeTypeU_LUI)
{
    // asm: lui x10, 0x12345
    rawInstr = 0x12345537;
    DecodedInstruction decInst = decoder.decode(rawInstr);

    EXPECT_EQ(decInst.id, InstructionId::LUI);
    EXPECT_EQ(decInst.format, Format::U);
    EXPECT_EQ(decInst.rd, 10);
    EXPECT_EQ(decInst.imm, 0x12345000);
}

TEST_F(DecoderTest, DecodeTypeR_ADD_SUB)
{
    // asm: add x1, x2, x3
    rawInstr = 0x003100B3;
    DecodedInstruction instAdd = decoder.decode(rawInstr);
    EXPECT_EQ(instAdd.id, InstructionId::ADD);
    EXPECT_EQ(instAdd.format, Format::R);
    EXPECT_EQ(instAdd.rd, 1);
    EXPECT_EQ(instAdd.rs1, 2);
    EXPECT_EQ(instAdd.rs2, 3);

    // asm: sub x1, x2, x3
    rawInstr = 0x403100B3;
    DecodedInstruction instSub = decoder.decode(rawInstr);
    EXPECT_EQ(instSub.id, InstructionId::SUB);
}

TEST_F(DecoderTest, DecodeTypeS_SW)
{
    // asm: sw x2, 12(x3)
    rawInstr = 0x0021A623;
    DecodedInstruction decInst = decoder.decode(rawInstr);

    EXPECT_EQ(decInst.id, InstructionId::SW);
    EXPECT_EQ(decInst.format, Format::S);
    EXPECT_EQ(decInst.rs1, 3);
    EXPECT_EQ(decInst.rs2, 2);
    EXPECT_EQ(decInst.imm, 12);
}

TEST_F(DecoderTest, DecodeTypeB_BNE_NegativeImm)
{
    // asm: bne x4, x5, -8
    rawInstr = 0xFE521CE3;
    DecodedInstruction decInst = decoder.decode(rawInstr);

    EXPECT_EQ(decInst.id, InstructionId::BNE);
    EXPECT_EQ(decInst.format, Format::B);
    EXPECT_EQ(decInst.rs1, 4);
    EXPECT_EQ(decInst.rs2, 5);
    EXPECT_EQ(decInst.imm, -8);
}

TEST_F(DecoderTest, DecodeTypeJ_JAL_NegativeImm)
{
    // asm: jal x1, -4
    rawInstr = 0xFFDFF0EF;
    DecodedInstruction decInst = decoder.decode(rawInstr);

    EXPECT_EQ(decInst.id, InstructionId::JAL);
    EXPECT_EQ(decInst.format, Format::J);
    EXPECT_EQ(decInst.rd, 1);
    EXPECT_EQ(decInst.imm, -4);
}

TEST_F(DecoderTest, Decode_UnknownIllegal)
{
    rawInstr = 0xFFFFFFFF;
    DecodedInstruction decInst = decoder.decode(rawInstr);

    EXPECT_EQ(decInst.id, InstructionId::Unknown);
    EXPECT_EQ(decInst.format, Format::Unknown);
}

TEST_F(DecoderTest, Decode_TypeI_Instructions)
{
    // rawInstr = 0xFF6F8513;

    // addi x10, x31, -10
    setOpcode(0b0010011);
    setFunct3(0b000);
    setRd(10);
    setRs1(31);
    setImmI(-10);

    DecodedInstruction decInst = decoder.decode(rawInstr);
    EXPECT_EQ(decInst.id, InstructionId::ADDI);
    EXPECT_EQ(decInst.format, Format::I);
    EXPECT_EQ(decInst.rd, 10);
    EXPECT_EQ(decInst.rs1, 31);

    // slti x10, x4, 2047
    setFunct3(0b010);
    setRs1(4);
    setImmI(2047);
    decInst = decoder.decode(rawInstr);
    EXPECT_EQ(decInst.id, InstructionId::SLTI);
    EXPECT_EQ(decInst.format, Format::I);
    EXPECT_EQ(decInst.rd, 10);
    EXPECT_EQ(decInst.rs1, 4);
    EXPECT_EQ(decInst.imm, 2047);

    // sltiu x10, x4, -2048
    setFunct3(0b011);
    setImmI(-2048);
    decInst = decoder.decode(rawInstr);
    EXPECT_EQ(decInst.id, InstructionId::SLTIU);
    EXPECT_EQ(decInst.format, Format::I);
    EXPECT_EQ(decInst.imm, -2048);

    // xori x10, x4, -10
    setFunct3(0b100);
    setImmI(-10);
    decInst = decoder.decode(rawInstr);
    EXPECT_EQ(decInst.id, InstructionId::XORI);
    EXPECT_EQ(decInst.format, Format::I);

    // ori x10, x4, -10
    setFunct3(0b110);
    decInst = decoder.decode(rawInstr);
    EXPECT_EQ(decInst.id, InstructionId::ORI);
    EXPECT_EQ(decInst.format, Format::I);

    // andi x10, x4, -10
    setFunct3(0b111);
    decInst = decoder.decode(rawInstr);
    EXPECT_EQ(decInst.id, InstructionId::ANDI);
    EXPECT_EQ(decInst.format, Format::I);

    // slli x10, x4, 30
    setFunct3(0b001);
    setImmI(30);
    EXPECT_EQ(rawInstr, 0x01E21513);
    decInst = decoder.decode(rawInstr);
    EXPECT_EQ(decInst.id, InstructionId::SLLI);
    EXPECT_EQ(decInst.format, Format::I);

    // srli x10, x4, 30
    setFunct3(0b101);
    decInst = decoder.decode(rawInstr);
    EXPECT_EQ(decInst.id, InstructionId::SRLI);
    EXPECT_EQ(decInst.format, Format::I);

    // srai x10, x4, 30
    setFunct3(0b101);
    rawInstr |= (1 << 30);
    decInst = decoder.decode(rawInstr);
    EXPECT_EQ(decInst.id, InstructionId::SRAI);
    EXPECT_EQ(decInst.format, Format::I);

    // Not type I
    setOpcode(0b0110011); // opcode of type R instructions
    setFunct3(0b011);
    setImmI(10);
    decInst = decoder.decode(rawInstr);
    EXPECT_NE(decInst.format, Format::I);

    // UnknownInstr
    setOpcode(0b0010011);
    setFunct3(0b101);
    setImmI(32); // immediate of shift instructions must be 0 to 31
    decInst = decoder.decode(rawInstr);
    EXPECT_EQ(decInst.id, InstructionId::Unknown);
    EXPECT_EQ(decInst.format, Format::Unknown);
}
