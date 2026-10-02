#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "execution_unit.h"
#include "ram.h"

using namespace RV32;
using namespace Core;

class InstrBuilder
{
private:
    DecodedInstruction instr;
public:
    InstrBuilder(InstructionId id, Format f)
    {
        instr.id = id;
        instr.format = f;
    }

    InstrBuilder& rd(uint8_t r)
    {
        instr.rd = r;
        return *this;
    }

    InstrBuilder& rs1(uint8_t r)
    {
        instr.rs1 = r;
        return *this;
    }

    InstrBuilder& rs2(uint8_t r)
    {
        instr.rs2 = r;
        return *this;
    }

    InstrBuilder& imm(int32_t i)
    {
        instr.imm = i;
        return *this;
    }

    operator DecodedInstruction() const { return instr; }
};

class ExecutionUnitTest : public ::testing::Test
{
protected:
    RegisterFile regFile;
    Ram memory{4, 4096, RamUninitBehavior::ZeroFill, Primitives::Endianness::Little};
    ExecutionUnit eu{regFile, memory};
    std::vector<DecodedInstruction> program;

    void SetUp() override
    {
        regFile.resetAllGPR();
        regFile.setPC(0);
    }

    ExecutionStatus exec(DecodedInstruction instr)
    {
        return eu.exec(instr);
    }

    // halts if EBREAK executed or cycles limit exceeded
    bool execProgram(int limit=1000)
    {
        int cycles = 0;

        while (cycles < limit)
        {
            uint32_t pc = regFile.getPC();
            size_t instrIndex = pc / 4;

            // PC is not aligned or PC exceeded instruction memory limit
            if ((pc % 4 != 0) || (instrIndex >= program.size()))
            {
                return false;
            }

            ExecutionStatus status = eu.exec(program[instrIndex]);

            // check error codes
            if (static_cast<uint16_t>(status) >=
                static_cast<uint16_t>(ExecutionStatus::Error)
                )
            {
                return false;
            }

            // successful program termination
            if (
                (status == ExecutionStatus::BreakpointHit) ||
                (status == ExecutionStatus::Halted)
                )
            {
                return true;
            }

            if (status == ExecutionStatus::Ok || status == ExecutionStatus::EnvCall)
            {
                regFile.advancePC(4);
            }
            else if (status != ExecutionStatus::BranchTaken &&
                     status != ExecutionStatus::JumpTaken &&
                     status != ExecutionStatus::PcChanged)
            {
                return false;
            }

            cycles++;
        }

        return false;
    }

    bool isSuccessStatus(ExecutionStatus status)
    {
        return static_cast<uint16_t>(status) <
               static_cast<uint16_t>(ExecutionStatus::Error);
    }
};

#define _R(_id, _rd, _rs1, _rs2) \
    InstrBuilder(InstructionId::_id, Format::R).rd(_rd).rs1(_rs1).rs2(_rs2)

#define _I(_id, _rd, _rs1, _imm) \
    InstrBuilder(InstructionId::_id, Format::I).rd(_rd).rs1(_rs1).imm(_imm)

#define _S(_id, _rs1, _rs2, _imm) \
    InstrBuilder(InstructionId::_id, Format::S).rs1(_rs1).rs2(_rs2).imm(_imm)

#define _B(_id, _rs1, _rs2, _imm) \
    InstrBuilder(InstructionId::_id, Format::B).rs1(_rs1).rs2(_rs2).imm(_imm)

#define _U(_id, _rd, _imm) \
    InstrBuilder(InstructionId::_id, Format::U).rd(_rd).imm(_imm)

#define _J(_id, _rd, _imm) \
    InstrBuilder(InstructionId::_id, Format::J).rd(_rd).imm(_imm)

#define _SYS(_id) \
    InstrBuilder(InstructionId::_id, Format::I)

// Execute
#define _Rx(_id, _rd, _rs1, _rs2)  exec(_R(_id, _rd, _rs1, _rs2))
#define _Ix(_id, _rd, _rs1, _imm)  exec(_I(_id, _rd, _rs1, _imm))
#define _Sx(_id, _rs1, _rs2, _imm) exec(_S(_id, _rs1, _rs2, _imm))
#define _Bx(_id, _rs1, _rs2, _imm) exec(_B(_id, _rs1, _rs2, _imm))
#define _Ux(_id, _rd, _imm)        exec(_U(_id, _rd, _imm))
#define _Jx(_id, _rd, _imm)        exec(_J(_id, _rd, _imm))
#define _SYSx(_id)                 exec(_SYS(_id))

// Execute and check result
#define _Rxc(_id, _rd, _rs1, _rs2)  EXPECT_TRUE(isSuccessStatus(_Rx(_id, _rd, _rs1, _rs2)))
#define _Ixc(_id, _rd, _rs1, _imm)  EXPECT_TRUE(isSuccessStatus(_Ix(_id, _rd, _rs1, _imm)))
#define _Sxc(_id, _rs1, _rs2, _imm) EXPECT_TRUE(isSuccessStatus(_Sx(_id, _rs1, _rs2, _imm)))
#define _Bxc(_id, _rs1, _rs2, _imm) EXPECT_TRUE(isSuccessStatus(_Bx(_id, _rs1, _rs2, _imm)))
#define _Uxc(_id, _rd, _imm)        EXPECT_TRUE(isSuccessStatus(_Ux(_id, _rd, _imm)))
#define _Jxc(_id, _rd, _imm)        EXPECT_TRUE(isSuccessStatus(_Jx(_id, _rd, _imm)))
#define _SYSxc(_id)                 EXPECT_TRUE(isSuccessStatus(_SYSx(_id)))

TEST_F(ExecutionUnitTest, BasicAluImmediateTest)
{
    // x1 = 0 + 42
    _Ixc(ADDI, 1, 0, 42);
    EXPECT_EQ(regFile.readGPR(1), 42);

    // x2 = x1 + (-12) = 30
    _Ixc(ADDI, 2, 1, -12);
    EXPECT_EQ(regFile.readGPR(2), 30);

    // 0b11110 & 0b1011 = 0b1010 (10)
    _Ixc(ANDI, 3, 2, 0b1011);
    EXPECT_EQ(regFile.readGPR(3), 10);

    // 10 | 5 = 15
    _Ixc(ORI, 4, 3, 5);
    EXPECT_EQ(regFile.readGPR(4), 15);

    // 15 ^ 15 = 0
    _Ixc(XORI, 5, 4, 15);
    EXPECT_EQ(regFile.readGPR(5), 0);
}

TEST_F(ExecutionUnitTest, BasicAluRegisterTest)
{
    _Ixc(ADDI, 1, 0, 100);
    _Ixc(ADDI, 2, 0, 35);

    // x3 = 100 + 35 = 135
    _Rxc(ADD, 3, 1, 2);
    EXPECT_EQ(regFile.readGPR(3), 135);

    // x4 = 100 - 35 = 65
    _Rxc(SUB, 4, 1, 2);
    EXPECT_EQ(regFile.readGPR(4), 65);

    // x5 = 100 ^ 35 = 71
    _Rxc(XOR, 5, 1, 2);
    EXPECT_EQ(regFile.readGPR(5), 71);

    // x6 = 100 | 35 = 103
    _Rxc(OR, 6, 1, 2);
    EXPECT_EQ(regFile.readGPR(6), 103);

    // x7 = 100 & 35 = 32
    _Rxc(AND, 7, 1, 2);
    EXPECT_EQ(regFile.readGPR(7), 32);
}

TEST_F(ExecutionUnitTest, RightLeftShiftsTest)
{
    // x1 = 1
    _Ixc(ADDI, 1, 0, 1);

    // SLLI: 1 << 4 = 16
    _Ixc(SLLI, 2, 1, 4);
    EXPECT_EQ(regFile.readGPR(2), 16);

    // SRLI: 16 >> 2 = 4
    _Ixc(SRLI, 3, 2, 2);
    EXPECT_EQ(regFile.readGPR(3), 4);

    // x4 = 3, x5 = x2 << x4 (16 << 3 = 128)
    _Ixc(ADDI, 4, 0, 3);
    _Rxc(SLL, 5, 2, 4);
    EXPECT_EQ(regFile.readGPR(5), 128);

    // x6 = -32 (0xFFFFFFE0)
    _Ixc(ADDI, 6, 0, -32);

    // SRAI: -32 >> 2 = -8 (0xFFFFFFF8)
    _Ixc(SRAI, 7, 6, 2);
    EXPECT_EQ(static_cast<int32_t>(regFile.readGPR(7)), -8);

    // SRA: -8 >> 1 = -4
    _Ixc(ADDI, 8, 0, 1);
    _Rxc(SRA, 9, 7, 8);
    EXPECT_EQ(static_cast<int32_t>(regFile.readGPR(9)), -4);
}

TEST_F(ExecutionUnitTest, SetLessThanTest)
{
    // x1 = -10 (0xFFFFFFF6), x2 = 5
    _Ixc(ADDI, 1, 0, -10);
    _Ixc(ADDI, 2, 0, 5);

    // -10 < 5 -> TRUE (1)
    _Rxc(SLT, 3, 1, 2);
    EXPECT_EQ(regFile.readGPR(3), 1);

    // -10 < 1 -> TRUE (1)
    _Ixc(SLTI, 4, 1, 5);
    EXPECT_EQ(regFile.readGPR(4), 1);

    // 0xFFFFFFF6 < 5 -> FALSE (0),
    _Rxc(SLTU, 5, 1, 2);
    EXPECT_EQ(regFile.readGPR(5), 0);

    // 0xFFFFFFF6 < 1 -> FALSE (0)
    _Ixc(SLTIU, 6, 1, 5);
    EXPECT_EQ(regFile.readGPR(6), 0);

    // 5 < -10 -> FALSE (0)
    _Rxc(SLT, 7, 2, 1);
    EXPECT_EQ(regFile.readGPR(7), 0);

    // 5 < 0xFFFFFFF6 -> TRUE (1)
    _Rxc(SLTU, 8, 2, 1);
    EXPECT_EQ(regFile.readGPR(8), 1);
}

TEST_F(ExecutionUnitTest, UTypeInstrTest)
{
    // --- LUI Test ---
    // x1 = 0x12345 << 12
    _Uxc(LUI, 1, 0x12345 << 12);
    EXPECT_EQ(regFile.readGPR(1), 0x12345000);

    // LUI + ADDI
    _Ixc(ADDI, 1, 1, 0x178);
    EXPECT_EQ(regFile.readGPR(1), 0x12345178);

    // --- AUIPC Test ---
    uint32_t testPc = 0x4000;
    regFile.setPC(testPc);

    // x2 = PC + (0x5 << 12) = 0x4000 + 0x5000 = 0x9000
    _Uxc(AUIPC, 2, 0x5 << 12);
    EXPECT_EQ(regFile.readGPR(2), 0x9000);

    _Ixc(ADDI, 2, 2, 0x123); // x2 = 0x9000 + 0x123 = 0x9123
    EXPECT_EQ(regFile.readGPR(2), testPc + 0x5123);

    // --- AUIPC with negative offset ---
    regFile.setPC(0x10000);
    // PC - 0x1000 = 0x10000 - 0x1000 = 0xF000
    _Uxc(AUIPC, 3, 0xFFFFF << 12);
    EXPECT_EQ(regFile.readGPR(3), 0xF000);
}

TEST_F(ExecutionUnitTest, SystemInstrTest)
{
    EXPECT_EQ(_SYSx(ECALL), ExecutionStatus::EnvCall);
    EXPECT_EQ(_SYSx(EBREAK), ExecutionStatus::BreakpointHit);
}

TEST_F(ExecutionUnitTest, JumpInstrTest)
{
    regFile.setPC(10*4);
    uint32_t oldPc = regFile.getPC();
    int32_t jumpOffset = -12;

    // jal x1, jumpOffset
    EXPECT_EQ(_Jx(JAL, 1, jumpOffset), ExecutionStatus::JumpTaken);
    EXPECT_EQ(regFile.getPC(), oldPc + jumpOffset);

    // unaligned jump
    oldPc = regFile.getPC();
    jumpOffset = 2;
    EXPECT_EQ(_Jx(JAL, 1, jumpOffset), ExecutionStatus::MisalignedAccess);
    EXPECT_EQ(regFile.getPC(), oldPc); // PC didn't change

    uint32_t jumpRegValue = 400;
    regFile.setPC(10*4);
    regFile.writeGPR(2, jumpRegValue);
    oldPc = regFile.getPC();
    jumpOffset = -12;
    // jalr x1, x2, jumpOffset
    EXPECT_EQ(_Ix(JALR, 1, 2, jumpOffset), ExecutionStatus::JumpTaken);
    EXPECT_EQ(regFile.getPC(), jumpOffset + jumpRegValue);

    // return to x1 address
    EXPECT_EQ(_Ix(JALR, 0, 1, 0), ExecutionStatus::JumpTaken);
    EXPECT_EQ(regFile.getPC(), oldPc + 4);

    // unaligned jump
    oldPc = regFile.getPC();
    jumpRegValue = 22;
    regFile.writeGPR(2, jumpRegValue);
    EXPECT_EQ(_Ix(JALR, 1, 2, jumpOffset), ExecutionStatus::MisalignedAccess);
    EXPECT_EQ(regFile.getPC(), oldPc); // PC didn't change

    // not unaligned jump
    oldPc = regFile.getPC();
    jumpRegValue = 0b101101; // JALR must reset least significant bit to 0
    regFile.writeGPR(2, jumpRegValue);
    EXPECT_EQ(_Ix(JALR, 1, 2, jumpOffset), ExecutionStatus::JumpTaken);
    EXPECT_EQ(regFile.getPC(), jumpOffset + (jumpRegValue & ~0x1));
}

TEST_F(ExecutionUnitTest, BranchInstrTest)
{
    uint32_t startPc = 0x100;
    int32_t branchOffset = 24;

    _Ixc(ADDI, 1, 0, 10);
    _Ixc(ADDI, 2, 0, 10);
    _Ixc(ADDI, 3, 0, 20);
    _Ixc(ADDI, 4, 0, -15);
    _Ixc(ADDI, 5, 0, 15);

    // --- BEQ/BNE test ---
    regFile.setPC(startPc);

    // BEQ: 10 == 10 -> branch
    EXPECT_EQ(_Bx(BEQ, 1, 2, branchOffset), ExecutionStatus::BranchTaken);
    EXPECT_EQ(regFile.getPC(), startPc + branchOffset);

    regFile.setPC(startPc);

    // BEQ: 10 == 20 -> no branch
    EXPECT_EQ(_Bx(BEQ, 1, 3, branchOffset), ExecutionStatus::Ok);
    EXPECT_EQ(regFile.getPC(), startPc); // PC didn't change

    // BNE: 10 != 20 -> branch
    EXPECT_EQ(_Bx(BNE, 1, 3, branchOffset), ExecutionStatus::BranchTaken);
    EXPECT_EQ(regFile.getPC(), startPc + branchOffset);

    regFile.setPC(startPc);

    // BNE: 10 != 10 -> no branch
    EXPECT_EQ(_Bx(BNE, 1, 2, branchOffset), ExecutionStatus::Ok);


    // --- BLT/BGE test (signed) ---
    regFile.setPC(startPc);

    // BLT: -15 < 15 -> branch
    EXPECT_EQ(_Bx(BLT, 4, 5, branchOffset), ExecutionStatus::BranchTaken);

    regFile.setPC(startPc);

    // BLT: 15 < -15 -> no branch
    EXPECT_EQ(_Bx(BLT, 5, 4, branchOffset), ExecutionStatus::Ok);

    // BGE: 15 >= -15 -> branch
    EXPECT_EQ(_Bx(BGE, 5, 4, branchOffset), ExecutionStatus::BranchTaken);

    regFile.setPC(startPc);

    // BGE: 15 >= 15 -> branch
    EXPECT_EQ(_Bx(BGE, 5, 5, branchOffset), ExecutionStatus::BranchTaken);

    regFile.setPC(startPc);

    // BGE: -15 >= 15 -> no branch
    EXPECT_EQ(_Bx(BGE, 4, 5, branchOffset), ExecutionStatus::Ok);


    // --- BLTU/BGEU (unsigned) ---
    regFile.setPC(startPc);

    // BLTU: 0xFFFFFFF1 < 15 -> no branch
    EXPECT_EQ(_Bx(BLTU, 4, 5, branchOffset), ExecutionStatus::Ok);

    // BLTU: 15 < 0xFFFFFFF1 -> branch
    EXPECT_EQ(_Bx(BLTU, 5, 4, branchOffset), ExecutionStatus::BranchTaken);
    regFile.setPC(startPc);

    // BGEU: 0xFFFFFFF1 >= 15 -> branch
    EXPECT_EQ(_Bx(BGEU, 4, 5, branchOffset), ExecutionStatus::BranchTaken);
    regFile.setPC(startPc);

    // BGEU: 15 >= 0xFFFFFFF1 -> no branch
    EXPECT_EQ(_Bx(BGEU, 5, 4, branchOffset), ExecutionStatus::Ok);


    // --- MisalignedAccess ---
    regFile.setPC(startPc);
    int32_t unalignedOffset = 6;

    // BEQ: 10 == 10
    EXPECT_EQ(_Bx(BEQ, 1, 2, unalignedOffset), ExecutionStatus::MisalignedAccess);
    EXPECT_EQ(regFile.getPC(), startPc); // PC didn't change
}


TEST_F(ExecutionUnitTest, MemoryLoadStoreTest)
{
    // set base address x1 = 0x200
    _Ixc(ADDI, 1, 0, 0x200);

    // x2 = 0xAABBCCDD
    regFile.writeGPR(2, 0xAABBCCDD);

    // --- Word access (SW/LW) ---
    // store 32 bits (0xAABBCCDD) at x1 + 0 = 0x200
    _Sxc(SW, 1, 2, 0);

    // load back into x3
    _Ixc(LW, 3, 1, 0);
    EXPECT_EQ(regFile.readGPR(3), 0xAABBCCDD);

    // --- Halfword access (SH/LH/LHU) ---
    // store the lowest 16 bits (0xCCDD) at x1 + 4 = 0x204
    _Sxc(SH, 1, 2, 4);

    // signed load (LH)
    // 0xCCDD must be sign extended to 0xFFFFCCDD
    _Ixc(LH, 4, 1, 4);
    EXPECT_EQ(regFile.readGPR(4), 0xFFFFCCDD);

    // unsigned load (LHU)
    // 0xCCDD must be zero-extended to 0x0000CCDD
    _Ixc(LHU, 5, 1, 4);
    EXPECT_EQ(regFile.readGPR(5), 0x0000CCDD);


    // --- Byte access (SB/LB/LBU) ---
    // store the lowest byte (0xDD) at x1 + 8 = 0x208
    _Sxc(SB, 1, 2, 8);

    // signed load (LB)
    // 0xDD (1101 1101) must be sign extended to 0xFFFFFFDD
    _Ixc(LB, 6, 1, 8);
    EXPECT_EQ(regFile.readGPR(6), 0xFFFFFFDD);

    // unsigned load (LBU)
    // 0xDD must be zero-extended to 0x000000DD
    _Ixc(LBU, 7, 1, 8);
    EXPECT_EQ(regFile.readGPR(7), 0x000000DD);

    // --- Unaligned Access Supported ---
    // store 0x11223344 at unaligned address 0x201 (x1 + 1)
    regFile.writeGPR(8, 0x11223344);
    EXPECT_EQ(_Sx(SW, 1, 8, 1), ExecutionStatus::Ok);

    // load the same word back
    _Ixc(LW, 9, 1, 1);
    EXPECT_EQ(regFile.readGPR(9), 0x11223344);

    // Little Endian:
    // 0x201: 0x44 (LSB)
    // 0x202: 0x33
    // 0x203: 0x22
    // 0x204: 0x11 (MSB)
    _Ixc(LBU, 10, 1, 1); // 0x201
    EXPECT_EQ(regFile.readGPR(10), 0x44);

    _Ixc(LBU, 11, 1, 4); // 0x204
    EXPECT_EQ(regFile.readGPR(11), 0x11);
}

TEST_F(ExecutionUnitTest, ManualDivisionProgram)
{
    uint32_t dividend = 20;
    uint32_t divisor = 3;

    // x10 - dividend; x11 - divisor
    // x12 - quotient; x13 - remainder
    program = {
        _I(ADDI, 10, 0, dividend),
        _I(ADDI, 11, 0, divisor),

        _I(ADDI, 12, 0, 0),  // Q = 0
        _I(ADDI, 13, 0, 0),  // R = 0
        _I(ADDI, 14, 0, 31), // loop counter

    // LOOP_START:
        _I(SLLI, 13, 13, 1), // remainder = remainder << 1

        // temp = (dividend >> i) & 1
        _R(SRL, 15, 10, 14), // x15 = N >> i
        _I(ANDI, 15, 15, 1), // x15 = x15 & 1

        _R(OR, 13, 13, 15), // remainder = remainder | temp

        _B(BLTU, 13, 11, 5*4), // if (remainder < divisor) JUMP to SKIP_SUB

        // else, if remainder >= divisor:
        _R(SUB, 13, 13, 11), // remainder -= divisor

        _I(ADDI, 16, 0, 1),  // x16 = 1
        _R(SLL, 16, 16, 14), // x16 = 1 << i
        _R(OR, 12, 12, 16),  // Q |= x16

    // SKIP_SUB:
        _I(ADDI, 14, 14, -1),   // i -= 1
        _I(ADDI, 17, 0, -1),    // x17 = -1
        _B(BNE, 14, 17, -11*4), // if (i != -1) JUMP to LOOP_START
        _SYS(EBREAK) // terminate program
    };

    EXPECT_TRUE(execProgram());

    EXPECT_EQ(regFile.readGPR(12), dividend / divisor);
    EXPECT_EQ(regFile.readGPR(13), dividend % divisor);
}

TEST_F(ExecutionUnitTest, ManualUnsignedMultiplyProgram)
{
    int32_t multiplicand = 37;
    int32_t multiplier = 56;

    program = {
    // MULTIPLY:
        // - result is stored in x31 and overflow is ignored
        // - multiplicand (A) and multiplier (B) written in x29 and x30 registers
        _I(ADDI, 29, 0, multiplicand),
        _I(ADDI, 30, 0, multiplier),

        _I(ADDI, 31, 0, 0), // x31 = 0
    // MUL_LOOP:
        _I(ANDI, 20, 30, 0x1), // x20 = x30[0]
        _B(BEQ, 20, 0, 2*4),   // if (x20 == 0) JUMP TO SKIP_ADD
        _R(ADD, 31, 31, 29),   // x31 = x31 + A

    // SKIP_ADD:
        _I(SLLI, 29, 29, 0x1), // A = A << 1
        _I(SRLI, 30, 30, 0x1), // B = B >> 1
        _B(BLT, 0, 30, -5*4),  // if (0 < B) JUMP TO MUL_LOOP
        _SYS(EBREAK)           // terminate program
    };

    EXPECT_TRUE(execProgram());
    EXPECT_EQ(regFile.readGPR(31), multiplicand * multiplier);
}

TEST_F(ExecutionUnitTest, SieveOfEratosthenesTest)
{
    regFile.resetAllGPR();
    regFile.setPC(0);

    // algorithm processes 1001 elements (from 0 to 1000)
    uint32_t limit = 1000;
    uint32_t baseAddress = 0x100;

    program = {
        // --- Prepare constants ---
        _I(ADDI, 28, 0, limit),       // x28 = limit
        _I(ADDI, 11, 0, baseAddress), // x11 = baseAddress
        _I(ADDI, 12, 0, 0),           // x12 = 0 (index)
        _I(ADDI, 18, 0, 1),           // x18 = 1 (True)

        // --- Fill array with values 1 ---
        // CreateTrueArrayLoop:
        _I(SLLI, 13, 12, 2),          // x13 = i * 4
        _R(ADD, 14, 11, 13),          // x14 = baseAddress + (i * 4)
        _S(SW, 14, 18, 0),            // mem[x14 + 0] = 1
        _I(ADDI, 12, 12, 1),          // i++
        // if (limit >= i) JUMP TO CreateTrueArrayLoop
        _B(BGE, 28, 12, -4 * 4),

        // --- Handle special cases (0 and 1 are not prime) ---
        // reset0And1:
        _S(SW, 11, 0, 0),     // mem[baseAddress + 0] = 0 (prime[0] = 0)
        _S(SW, 11, 0, 4),     // mem[baseAddress + 4] = 0 (prime[1] = 0)
        _I(ADDI, 12, 0, 2),   // start from i = 2

        // --- Main Sieve process ---
        // LoopOuter:
        _I(SLLI, 13, 12, 2),   // x13 = i * 4
        _R(ADD, 14, 11, 13),   // x14 = baseAddress + (i * 4)
        _I(LW, 19, 14, 0),     // x19 = mem[x14]

        // inner loop starts with j = i * 2, so calculate i + i
        _R(ADD, 15, 12, 12),    // x15 (j) = i + i

        // if the flag is 1, the number is prime -> jump to the inner loop
        _B(BEQ, 18, 19, 4 * 4),

        // Increment:
        _I(ADDI, 12, 12, 1),    // i++
        // if limit < i, jump to Exit
        _B(BLT, 28, 12, 8 * 4),
        _J(JAL, 0, -7 * 4),  // JUMP to LoopOuter

        // LoopInner:
        _I(SLLI, 16, 15, 2),     // x16 = j * 4
        _R(ADD, 29, 11, 16),     // x29 = baseAddress + (j * 4)
        _S(SW, 29, 0, 0),        // mem[x29 + 0] = 0
        _R(ADD, 15, 15, 12),     // j += i
        // if limit >= j, jump back to LoopInner
        _B(BGE, 28, 15, -4 * 4),
        _J(JAL, 0, -8 * 4),      // JUMP to Increment

        // Exit:
        _SYS(EBREAK)    // terminate program
    };

    // run the program with extra cycles
    ASSERT_TRUE(execProgram(30000));

    // --- Verify results (compare with the C++ implementation) ---
    std::vector<bool> expectedPrimes(limit + 1, true);
    expectedPrimes[0] = false;
    expectedPrimes[1] = false;
    for (uint32_t p = 2; p * p <= limit; p++)
    {
        if (expectedPrimes[p])
        {
            for (uint32_t i = p * p; i <= limit; i += p)
                expectedPrimes[i] = false;
        }
    }

    // read the simulator memory and compare every element
    for (uint32_t i = 0; i <= limit; i++)
    {
        uint32_t actualFlag = 0;
        uint32_t address = baseAddress + (i * 4);

        // read the 32-bit word written by the processor from memory
        BusStatus status = memory.read32(address, actualFlag);
        ASSERT_EQ(status, BusStatus::Ok) << "Memory read failed at address " << address;

        uint32_t expectedFlag = expectedPrimes[i] ? 1 : 0;
        EXPECT_EQ(actualFlag, expectedFlag)
            << "Mismatch at index " << i << " (Number " << i << " is "
            << (expectedPrimes[i] ? "prime" : "not prime") << ")";
    }
}
