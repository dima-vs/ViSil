#pragma once
#include <cstdint>
#include "primitives.h"

namespace Core
{

enum class BusStatus
{
    Ok,
    UnmappedAddress,
    AccessFault,
    MisalignedAccess
};

class IBusDevice
{
protected:
    Primitives::Endianness m_endianness;
public:
    virtual ~IBusDevice() = default;
    IBusDevice(Primitives::Endianness endianness) :
        m_endianness(endianness) {}

    // size in bytes
    virtual BusStatus readBlock(uint64_t addr, uint8_t* buf, size_t size) = 0;
    virtual BusStatus writeBlock(uint64_t addr, const uint8_t* buf, size_t size) = 0;

    virtual BusStatus read8(uint64_t addr, uint8_t& outVal)
    {
        return readBlock(addr, &outVal, 1);
    }

    virtual BusStatus read16(uint64_t addr, uint16_t& outVal)
    {
        uint8_t buf[2];
        BusStatus status = readBlock(addr, buf, 2);
        if (status != BusStatus::Ok)
            return status;

        if (m_endianness == Primitives::Endianness::Little)
        {
            outVal = static_cast<uint16_t>(buf[0]) |
                     (static_cast<uint16_t>(buf[1]) << 8);
        } else // Big Endian
        {
            outVal = (static_cast<uint16_t>(buf[0]) << 8) |
                     static_cast<uint16_t>(buf[1]);
        }
        return BusStatus::Ok;
    }

    virtual BusStatus read32(uint64_t addr, uint32_t& outVal)
    {
        uint8_t buf[4];
        BusStatus status = readBlock(addr, buf, 4);
        if (status != BusStatus::Ok)
            return status;

        if (m_endianness == Primitives::Endianness::Little)
        {
            outVal = static_cast<uint32_t>(buf[0]) |
                      (static_cast<uint32_t>(buf[1]) << 1*8)  |
                      (static_cast<uint32_t>(buf[2]) << 2*8) |
                      (static_cast<uint32_t>(buf[3]) << 3*8);
        } else // Big Endian
        {
            outVal = (static_cast<uint32_t>(buf[0]) << 3*8) |
                      (static_cast<uint32_t>(buf[1]) << 2*8) |
                      (static_cast<uint32_t>(buf[2]) << 1*8)  |
                      static_cast<uint32_t>(buf[3]);
        }
        return BusStatus::Ok;
    }
    virtual BusStatus read64(uint64_t addr, uint64_t& outVal)
    {
        uint8_t buf[8];
        BusStatus status = readBlock(addr, buf, 8);
        if (status != BusStatus::Ok)
            return status;

        if (m_endianness == Primitives::Endianness::Little)
        {
            outVal = static_cast<uint64_t>(buf[0]) |
                     (static_cast<uint64_t>(buf[1]) << 1*8)  |
                     (static_cast<uint64_t>(buf[2]) << 2*8) |
                     (static_cast<uint64_t>(buf[3]) << 3*8) |
                     (static_cast<uint64_t>(buf[4]) << 4*8) |
                     (static_cast<uint64_t>(buf[5]) << 5*8) |
                     (static_cast<uint64_t>(buf[6]) << 6*8) |
                     (static_cast<uint64_t>(buf[7]) << 7*8);
        } else // Big Endian
        {
            outVal = (static_cast<uint64_t>(buf[0]) << 7*8) |
                     (static_cast<uint64_t>(buf[1]) << 6*8) |
                     (static_cast<uint64_t>(buf[2]) << 5*8) |
                     (static_cast<uint64_t>(buf[3]) << 4*8) |
                     (static_cast<uint64_t>(buf[4]) << 3*8) |
                     (static_cast<uint64_t>(buf[5]) << 2*8) |
                     (static_cast<uint64_t>(buf[6]) << 1*8) |
                     static_cast<uint64_t>(buf[7]);
        }
        return BusStatus::Ok;
    }

    virtual BusStatus write8(uint64_t addr, uint8_t val)
    {
        return writeBlock(addr, &val, 1);
    }

    virtual BusStatus write16(uint64_t addr, uint16_t val)
    {
        uint8_t buf[2];
        if (m_endianness == Primitives::Endianness::Little)
        {
            buf[0] = static_cast<uint8_t>(val & 0xFF);
            buf[1] = static_cast<uint8_t>((val >> 8) & 0xFF);
        } else // Big Endian
        {
            buf[0] = static_cast<uint8_t>((val >> 8) & 0xFF);
            buf[1] = static_cast<uint8_t>(val & 0xFF);
        }
        return writeBlock(addr, buf, 2);
    }

    virtual BusStatus write32(uint64_t addr, uint32_t val)
    {
        uint8_t buf[4];
        if (m_endianness == Primitives::Endianness::Little)
        {
            buf[0] = static_cast<uint8_t>(val & 0xFF);
            buf[1] = static_cast<uint8_t>((val >> 1*8) & 0xFF);
            buf[2] = static_cast<uint8_t>((val >> 2*8) & 0xFF);
            buf[3] = static_cast<uint8_t>((val >> 3*8) & 0xFF);
        } else // Big Endian
        {
            buf[0] = static_cast<uint8_t>((val >> 3*8) & 0xFF);
            buf[1] = static_cast<uint8_t>((val >> 2*8) & 0xFF);
            buf[2] = static_cast<uint8_t>((val >> 1*8) & 0xFF);
            buf[3] = static_cast<uint8_t>(val & 0xFF);
        }
        return writeBlock(addr, buf, 4);
    }

    virtual BusStatus write64(uint64_t addr, uint64_t val)
    {
        uint8_t buf[8];
        if (m_endianness == Primitives::Endianness::Little)
        {
            buf[0] = static_cast<uint8_t>(val & 0xFF);
            buf[1] = static_cast<uint8_t>((val >> 1*8) & 0xFF);
            buf[2] = static_cast<uint8_t>((val >> 2*8) & 0xFF);
            buf[3] = static_cast<uint8_t>((val >> 3*8) & 0xFF);
            buf[4] = static_cast<uint8_t>((val >> 4*8) & 0xFF);
            buf[5] = static_cast<uint8_t>((val >> 5*8) & 0xFF);
            buf[6] = static_cast<uint8_t>((val >> 6*8) & 0xFF);
            buf[7] = static_cast<uint8_t>((val >> 7*8) & 0xFF);
        } else // Big Endian
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
        return writeBlock(addr, buf, 8);
    }
};

} // namespace Core
