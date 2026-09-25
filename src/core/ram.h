#pragma once
#include "bus_device_interface.h"
#include <cstdint>
#include <vector>
#include <memory>

namespace Core
{

enum class RamUninitBehavior
{
    ZeroFill
    // PatternFill,
    // RandomOnPowerOn,
    // HardwareFault
};

class Ram : public IBusDevice
{
private:
    // size of a single physically allocated block
    uint32_t m_blockSize;
    uint32_t m_blockCount;
    size_t m_capacity;

    std::vector<std::unique_ptr<uint8_t[]>> m_blocks;
    // holds true if the corresponding memory cell has been written at least once
    std::vector<std::unique_ptr<bool[]>> m_initializedMask;

    RamUninitBehavior m_ramBehavior;

    void allocateBlock(size_t blockIndex);
    BusStatus readBytesWithUninitBehavior(
        size_t blockIndex,
        size_t offset,
        uint8_t* destBuf,
        size_t bytesToRead
        );
public:
    Ram(
        size_t blockCount, size_t blockSize=4*1024,
        RamUninitBehavior ramBehavior=RamUninitBehavior::ZeroFill,
        Primitives::Endianness endianness = Primitives::Endianness::Little
        );

    BusStatus readBlock(uint64_t addr, uint8_t* buf, size_t size) override;
    BusStatus writeBlock(uint64_t addr, const uint8_t* buf, size_t size) override;
};

} // namespace Core
