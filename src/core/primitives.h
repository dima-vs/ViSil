#pragma once

#include <cstdint>

namespace Core::Primitives
{

enum class Endianness
{
    Little,
    Big
};

enum class BitWidth : uint16_t
{
    Bits8 = 8,
    Bits16 = 16,
    Bits32 = 32,
    Bits64 = 64,
    Bits128 = 128
};


enum class ArchFamily
{
    Risc,
    Cisc,
    Other
};

} // namespace Core::Primitives
