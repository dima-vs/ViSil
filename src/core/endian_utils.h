#pragma once
#include <cstdint>
#include "common.h"

namespace Core::Endian
{

constexpr uint16_t read16(const uint8_t* buf, Endianness end)
{
    if (end == Endianness::Little)
    {
        return static_cast<uint16_t>(buf[0]) |
               (static_cast<uint16_t>(buf[1]) << 8);
    }
    // Big Endian
    return (static_cast<uint16_t>(buf[0]) << 8) |
           static_cast<uint16_t>(buf[1]);
}

constexpr void write16(uint8_t* buf, uint16_t val, Endianness end)
{
    if (end == Endianness::Little)
    {
        buf[0] = static_cast<uint8_t>(val & 0xFF);
        buf[1] = static_cast<uint8_t>((val >> 8) & 0xFF);
    }
    else // Big Endian
    {
        buf[0] = static_cast<uint8_t>((val >> 8) & 0xFF);
        buf[1] = static_cast<uint8_t>(val & 0xFF);
    }
}

constexpr uint32_t read32(const uint8_t* buf, Endianness end)
{
    if (end == Endianness::Little)
    {
        return static_cast<uint32_t>(buf[0]) |
               (static_cast<uint32_t>(buf[1]) << 1*8) |
               (static_cast<uint32_t>(buf[2]) << 2*8) |
               (static_cast<uint32_t>(buf[3]) << 3*8);
    }
    // Big Endian
    return (static_cast<uint32_t>(buf[0]) << 3*8) |
           (static_cast<uint32_t>(buf[1]) << 2*8) |
           (static_cast<uint32_t>(buf[2]) << 1*8) |
           static_cast<uint32_t>(buf[3]);
}

constexpr void write32(uint8_t* buf, uint32_t val, Endianness end)
{
    if (end == Endianness::Little)
    {
        buf[0] = static_cast<uint8_t>(val & 0xFF);
        buf[1] = static_cast<uint8_t>((val >> 1*8) & 0xFF);
        buf[2] = static_cast<uint8_t>((val >> 2*8) & 0xFF);
        buf[3] = static_cast<uint8_t>((val >> 3*8) & 0xFF);
    }
    else // Big Endian
    {
        buf[0] = static_cast<uint8_t>((val >> 3*8) & 0xFF);
        buf[1] = static_cast<uint8_t>((val >> 2*8) & 0xFF);
        buf[2] = static_cast<uint8_t>((val >> 1*8) & 0xFF);
        buf[3] = static_cast<uint8_t>(val & 0xFF);
    }
}

constexpr uint64_t read64(const uint8_t* buf, Endianness end)
{
    if (end == Endianness::Little)
    {
        return static_cast<uint64_t>(buf[0]) |
            (static_cast<uint64_t>(buf[1]) << 1*8) |
            (static_cast<uint64_t>(buf[2]) << 2*8) |
            (static_cast<uint64_t>(buf[3]) << 3*8) |
            (static_cast<uint64_t>(buf[4]) << 4*8) |
            (static_cast<uint64_t>(buf[5]) << 5*8) |
            (static_cast<uint64_t>(buf[6]) << 6*8) |
            (static_cast<uint64_t>(buf[7]) << 7*8);
    }
    // Big Endian
    return (static_cast<uint64_t>(buf[0]) << 7*8) |
           (static_cast<uint64_t>(buf[1]) << 6*8) |
           (static_cast<uint64_t>(buf[2]) << 5*8) |
           (static_cast<uint64_t>(buf[3]) << 4*8) |
           (static_cast<uint64_t>(buf[4]) << 3*8) |
           (static_cast<uint64_t>(buf[5]) << 2*8) |
           (static_cast<uint64_t>(buf[6]) << 1*8) |
           static_cast<uint64_t>(buf[7]);
}

constexpr void write64(uint8_t* buf, uint64_t val, Endianness end)
{
    if (end == Endianness::Little)
    {
        buf[0] = static_cast<uint8_t>(val & 0xFF);
        buf[1] = static_cast<uint8_t>((val >> 1*8) & 0xFF);
        buf[2] = static_cast<uint8_t>((val >> 2*8) & 0xFF);
        buf[3] = static_cast<uint8_t>((val >> 3*8) & 0xFF);
        buf[4] = static_cast<uint8_t>((val >> 4*8) & 0xFF);
        buf[5] = static_cast<uint8_t>((val >> 5*8) & 0xFF);
        buf[6] = static_cast<uint8_t>((val >> 6*8) & 0xFF);
        buf[7] = static_cast<uint8_t>((val >> 7*8) & 0xFF);
    }
    else // Big Endian
    {
        buf[0] = static_cast<uint8_t>((val >> 7*8) & 0xFF);
        buf[1] = static_cast<uint8_t>((val >> 6*8) & 0xFF);
        buf[2] = static_cast<uint8_t>((val >> 5*8) & 0xFF);
        buf[3] = static_cast<uint8_t>((val >> 4*8) & 0xFF);
        buf[4] = static_cast<uint8_t>((val >> 3*8) & 0xFF);
        buf[5] = static_cast<uint8_t>((val >> 2*8) & 0xFF);
        buf[6] = static_cast<uint8_t>((val >> 1*8) & 0xFF);
        buf[7] = static_cast<uint8_t>(val & 0xFF);
    }
}

} // namespace Core::Endian
