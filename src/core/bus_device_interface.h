#pragma once
#include <cstdint>
#include "common.h"
#include "endian_utils.h"

namespace Core
{

class IBusDevice
{
protected:
    Endianness m_endianness;
public:
    virtual ~IBusDevice() = default;
    IBusDevice(Endianness endianness) :
        m_endianness(endianness) {}

    // address space size of a device in bytes
    virtual uint64_t getSize() const = 0;

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
        if (status == BusStatus::Ok)
            outVal = Endian::read16(buf, m_endianness);
        return status;
    }

    virtual BusStatus read32(uint64_t addr, uint32_t& outVal)
    {
        uint8_t buf[4];
        BusStatus status = readBlock(addr, buf, 4);
        if (status == BusStatus::Ok)
            outVal = Endian::read32(buf, m_endianness);
        return status;
    }

    virtual BusStatus read64(uint64_t addr, uint64_t& outVal)
    {
        uint8_t buf[8];
        BusStatus status = readBlock(addr, buf, 8);
        if (status == BusStatus::Ok)
            outVal = Endian::read64(buf, m_endianness);
        return status;
    }

    virtual BusStatus write8(uint64_t addr, uint8_t val)
    {
        return writeBlock(addr, &val, 1);
    }

    virtual BusStatus write16(uint64_t addr, uint16_t val)
    {
        uint8_t buf[2];
        Endian::write16(buf, val, m_endianness);
        return writeBlock(addr, buf, 2);
    }

    virtual BusStatus write32(uint64_t addr, uint32_t val)
    {
        uint8_t buf[4];
        Endian::write32(buf, val, m_endianness);
        return writeBlock(addr, buf, 4);
    }

    virtual BusStatus write64(uint64_t addr, uint64_t val)
    {
        uint8_t buf[8];
        Endian::write64(buf, val, m_endianness);
        return writeBlock(addr, buf, 8);
    }

    // Methods with no side effects.
    // Used for debugging and UI purposes.
    virtual BusStatus peekBlock(uint64_t addr, uint8_t* buf, size_t size) const = 0;

    virtual BusStatus peek8(uint64_t addr, uint8_t& outVal) const
    {
        return peekBlock(addr, &outVal, 1);
    }

    virtual BusStatus peek16(uint64_t addr, uint16_t& outVal) const
    {
        uint8_t buf[2];
        BusStatus status = peekBlock(addr, buf, 2);
        if (status == BusStatus::Ok)
            outVal = Endian::read16(buf, m_endianness);
        return status;
    }

    virtual BusStatus peek32(uint64_t addr, uint32_t& outVal) const
    {
        uint8_t buf[4];
        BusStatus status = peekBlock(addr, buf, 4);
        if (status == BusStatus::Ok)
            outVal = Endian::read32(buf, m_endianness);
        return status;
    }

    virtual BusStatus peek64(uint64_t addr, uint64_t& outVal) const
    {
        uint8_t buf[8];
        BusStatus status = peekBlock(addr, buf, 8);
        if (status == BusStatus::Ok)
            outVal = Endian::read64(buf, m_endianness);
        return status;
    }
};

} // namespace Core
