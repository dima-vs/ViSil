#pragma once

#include "disassembler_interface.h"
#include "decoder.h"

namespace RV32
{

class RV32Disassembler : public Core::IDisassembler
{
private:
    Decoder m_decoder;

    void addToken(Core::DisassembledInstr& instrOut, Core::TokenType type, const std::string& text) const;

    void buildRType(Core::DisassembledInstr& instrOut, const DecodedInstruction& dec) const;
    void buildIType(Core::DisassembledInstr& instrOut, const DecodedInstruction& dec) const;
    void buildLoad(Core::DisassembledInstr& instrOut, const DecodedInstruction& dec) const;
    void buildStore(Core::DisassembledInstr& instrOut, const DecodedInstruction& dec) const;
    void buildBType(Core::DisassembledInstr& instrOut, const DecodedInstruction& dec, uint64_t pc) const;
    void buildUType(Core::DisassembledInstr& instrOut, const DecodedInstruction& dec) const;
    void buildJType(Core::DisassembledInstr& instrOut, const DecodedInstruction& dec, uint64_t pc) const;
    void buildSystem(Core::DisassembledInstr& instrOut, const DecodedInstruction& dec) const;

    std::string getMnemonic(InstructionId id) const;
    std::string getRegName(uint8_t regIndex) const;


    bool isOpcodeOnlyInstr(const DecodedInstruction& dec) const;
    bool isLoadInstr(const DecodedInstruction& dec) const;
public:
    Core::DisassembledInstr disassemble(
        uint64_t address,
        const uint8_t* data,
        size_t maxSize
        ) const override;
};

} // namespace RV32
