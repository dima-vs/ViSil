#include "rv32_disassembler.h"
#include "register_file.h"
#include <iomanip>
#include <sstream>
#include <array>

namespace RV32
{

void RV32Disassembler::addToken(
    Core::DisassembledInstr& instrOut,
    Core::TokenType type, const std::string& text
    ) const
{
    instrOut.tokens.push_back({type, text});
}

std::string RV32Disassembler::getMnemonic(InstructionId id) const
{
    static const std::array<const char*, static_cast<size_t>(InstructionId::Count)> mnemonics = {
        "add", "sub", "xor", "or", "and", "sll", "srl", "sra", "slt", "sltu",
        "addi", "xori", "ori", "andi", "slli", "srli", "srai", "slti", "sltiu",
        "lb", "lh", "lw", "lbu", "lhu",
        "sb", "sh", "sw",
        "beq", "bne", "blt", "bge", "bltu", "bgeu",
        "jal", "jalr",
        "lui", "auipc",
        "ecall", "ebreak",
        "fence", "fence.tso", "pause"
    };

    size_t idx = static_cast<size_t>(id);
    if (idx < mnemonics.size())
    {
        return mnemonics[idx];
    }
    return "unknown";
}

std::string RV32Disassembler::getRegName(uint8_t regIndex) const
{
    return std::string(RegisterFile::getAbiName(regIndex));
}

Core::DisassembledInstr RV32Disassembler::disassemble(uint64_t address, const uint8_t* data, size_t maxSize) const
{
    Core::DisassembledInstr result;
    // RV32I instruction size is always 4
    result.sizeBytes = 4;

    if (maxSize < 4)
    {
        result.isIllegal = true;
        addToken(result, Core::TokenType::Mnemonic, "illegal");
        return result;
    }

    uint32_t rawInstr = static_cast<uint32_t>(data[0]) |
                        (static_cast<uint32_t>(data[1]) << 8) |
                        (static_cast<uint32_t>(data[2]) << 16) |
                        (static_cast<uint32_t>(data[3]) << 24);

    DecodedInstruction dec = m_decoder.decode(rawInstr);

    if (!Decoder::checkInstruction(dec.id))
    {
        result.isIllegal = true;
        addToken(result, Core::TokenType::Mnemonic, "illegal");
        return result;
    }

    addToken(result, Core::TokenType::Mnemonic, getMnemonic(dec.id));
    addToken(result, Core::TokenType::Whitespace, " ");

    if (isOpcodeOnlyInstr(dec))
    {
        // do nothing
    }
    else if (isLoadInstr(dec))
    {
        buildLoad(result, dec);
    }
    else if (dec.format == Format::S)
    {
        buildStore(result, dec);
    }
    else if (dec.format == Format::B)
    {
        buildBType(result, dec, address);
    }
    else if (dec.format == Format::J)
    {
        buildJType(result, dec, address);
    }
    else if (dec.format == Format::R)
    {
        buildRType(result, dec);
    }
    else if (dec.format == Format::I)
    {
        buildIType(result, dec);
    }
    else if (dec.format == Format::U)
    {
        buildUType(result, dec);
    }

    return result;
}

void RV32Disassembler::buildRType(Core::DisassembledInstr& instrOut, const DecodedInstruction& dec) const
{
    // opcode rd, rs1, rs2
    addToken(instrOut, Core::TokenType::Register, getRegName(dec.rd));
    addToken(instrOut, Core::TokenType::Punctuation, ", ");
    addToken(instrOut, Core::TokenType::Register, getRegName(dec.rs1));
    addToken(instrOut, Core::TokenType::Punctuation, ", ");
    addToken(instrOut, Core::TokenType::Register, getRegName(dec.rs2));
}

void RV32Disassembler::buildIType(Core::DisassembledInstr& instrOut, const DecodedInstruction& dec) const
{
    // opcode rd, rs1, imm
    addToken(instrOut, Core::TokenType::Register, getRegName(dec.rd));
    addToken(instrOut, Core::TokenType::Punctuation, ", ");
    addToken(instrOut, Core::TokenType::Register, getRegName(dec.rs1));
    addToken(instrOut, Core::TokenType::Punctuation, ", ");
    addToken(instrOut, Core::TokenType::Immediate, std::to_string(dec.imm));
}

void RV32Disassembler::buildUType(Core::DisassembledInstr& instrOut, const DecodedInstruction& dec) const
{
    // opcode rd, imm
    addToken(instrOut, Core::TokenType::Register, getRegName(dec.rd));
    addToken(instrOut, Core::TokenType::Punctuation, ", ");

    int32_t unshiftedImm = dec.imm >> 12;
    uint32_t displayImm = static_cast<uint32_t>(unshiftedImm) & 0xFFFFF;

    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << displayImm;
    addToken(instrOut, Core::TokenType::Immediate, ss.str());
}

void RV32Disassembler::buildJType(Core::DisassembledInstr& instrOut, const DecodedInstruction& dec, uint64_t pc) const
{
    // jal rd, offset
    addToken(instrOut, Core::TokenType::Register, getRegName(dec.rd));
    addToken(instrOut, Core::TokenType::Punctuation, ", ");

    // calculate absolute address for UI
    uint64_t targetAddr = pc + dec.imm;

    std::stringstream ss;
    ss << "0x" << std::setfill('0') << std::setw(8) << std::hex << std::uppercase << targetAddr;
    addToken(instrOut, Core::TokenType::Address, ss.str());
}

void RV32Disassembler::buildLoad(Core::DisassembledInstr& instrOut, const DecodedInstruction& dec) const
{
    // lw rd, imm(rs1)
    addToken(instrOut, Core::TokenType::Register, getRegName(dec.rd));
    addToken(instrOut, Core::TokenType::Punctuation, ", ");
    addToken(instrOut, Core::TokenType::Immediate, std::to_string(dec.imm));
    addToken(instrOut, Core::TokenType::Punctuation, "(");
    addToken(instrOut, Core::TokenType::Register, getRegName(dec.rs1));
    addToken(instrOut, Core::TokenType::Punctuation, ")");
}

void RV32Disassembler::buildStore(Core::DisassembledInstr& instrOut, const DecodedInstruction& dec) const
{
    // sw rs2, imm(rs1)
    addToken(instrOut, Core::TokenType::Register, getRegName(dec.rs2));
    addToken(instrOut, Core::TokenType::Punctuation, ", ");
    addToken(instrOut, Core::TokenType::Immediate, std::to_string(dec.imm));
    addToken(instrOut, Core::TokenType::Punctuation, "(");
    addToken(instrOut, Core::TokenType::Register, getRegName(dec.rs1));
    addToken(instrOut, Core::TokenType::Punctuation, ")");
}

void RV32Disassembler::buildBType(Core::DisassembledInstr& instrOut, const DecodedInstruction& dec, uint64_t pc) const
{
    // beq rs1, rs2, offset
    addToken(instrOut, Core::TokenType::Register, getRegName(dec.rs1));
    addToken(instrOut, Core::TokenType::Punctuation, ", ");
    addToken(instrOut, Core::TokenType::Register, getRegName(dec.rs2));
    addToken(instrOut, Core::TokenType::Punctuation, ", ");

    // calculate absolute address for UI
    uint64_t targetAddr = pc + dec.imm;

    std::stringstream ss;

    ss << "0x" << std::setfill('0') << std::setw(8) << std::hex << std::uppercase << targetAddr;
    addToken(instrOut, Core::TokenType::Address, ss.str());
}

bool RV32Disassembler::isOpcodeOnlyInstr(const DecodedInstruction& dec) const
{
    return (dec.id == InstructionId::ECALL) ||
           (dec.id == InstructionId::EBREAK) ||
           (dec.id == InstructionId::FENCE) ||
           (dec.id == InstructionId::FENCE_TSO) ||
           (dec.id == InstructionId::PAUSE);
}

bool RV32Disassembler::isLoadInstr(const DecodedInstruction& dec) const
{
    return (dec.id == InstructionId::LB) ||
           (dec.id == InstructionId::LH) ||
           (dec.id == InstructionId::LW) ||
           (dec.id == InstructionId::LBU) ||
           (dec.id == InstructionId::LHU);
}

} // namespace RV32
