#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <cstddef>
#include <algorithm>

namespace Core
{

enum class TokenType
{
    Mnemonic,
    Register,
    Immediate,
    Address,
    Punctuation,
    Whitespace,
    Incorrect
};

struct DisasmToken
{
    TokenType type;
    std::string text;
};

struct DisassembledInstr
{
    std::vector<DisasmToken> tokens;
    uint32_t sizeBytes = 0;
    bool isIllegal = false;

    std::string toString() const
    {
        std::string result;
        for (const auto& token : tokens)
            result += token.text;
        return result;
    }

    // returns the object without whitespace at the edges
    DisassembledInstr trimmed() const
    {
        DisassembledInstr result;
        result.sizeBytes = this->sizeBytes;
        result.isIllegal = this->isIllegal;

        if (tokens.empty())
        {
            return result;
        }

        auto isWhitespace = [](const DisasmToken& t) {
            return t.type == TokenType::Whitespace;
        };

        auto startIt = std::find_if_not(tokens.begin(), tokens.end(), isWhitespace);

        if (startIt == tokens.end())
        {
            return result;
        }

        auto endIt = std::find_if_not(tokens.rbegin(), tokens.rend(), isWhitespace).base();
        result.tokens.assign(startIt, endIt);

        return result;
    }
};

class IDisassembler
{
public:
    virtual ~IDisassembler() = default;

    virtual DisassembledInstr disassemble(
        uint64_t address,
        const uint8_t* data,
        size_t maxSize
        ) const = 0;
};

} // namespace Core
