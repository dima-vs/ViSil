#include "ram.h"
#include <algorithm>
#include <cstring>

namespace Core
{

Ram::Ram(
    size_t blockCount, size_t blockSize,
    RamUninitBehavior ramBehavior,
    Primitives::Endianness endianness) : IBusDevice(endianness),
    m_blockCount(blockCount),
    m_blockSize(blockSize),
    m_blocks(blockCount),
    m_initializedMask(blockCount),
    m_ramBehavior(ramBehavior)
{
    m_capacity = m_blockCount * m_blockSize;
}

BusStatus Ram::readBlock(uint64_t addr, uint8_t* buf, size_t size)
{
    if (addr >= m_capacity || size > m_capacity || size > (m_capacity - addr))
        return BusStatus::UnmappedAddress;

    size_t blockIndex = addr / m_blockSize;
    size_t offset = addr % m_blockSize;

    size_t bytesRead = 0;
    while (bytesRead < size)
    {
        size_t bytesToRead = std::min(size - bytesRead, m_blockSize - offset);

        BusStatus status = readBytesWithUninitBehavior(
            blockIndex,
            offset,
            buf + bytesRead,
            bytesToRead
            );

        if (status != BusStatus::Ok)
            return status;

        bytesRead += bytesToRead;
        ++blockIndex;
        offset = 0;
    }

    return BusStatus::Ok;
}

BusStatus Ram::writeBlock(uint64_t addr, const uint8_t* buf, size_t size)
{
    if (addr >= m_capacity || size > m_capacity || size > (m_capacity - addr))
        return BusStatus::UnmappedAddress;

    size_t blockIndex = addr / m_blockSize;
    size_t offset = addr % m_blockSize;

    size_t bytesWritten = 0;
    while (bytesWritten < size)
    {
        allocateBlock(blockIndex);

        uint8_t* blockBytes = m_blocks[blockIndex].get();
        size_t bytesToWrite = std::min(size - bytesWritten, m_blockSize - offset);
        std::memcpy(&blockBytes[offset], &buf[bytesWritten], bytesToWrite);

        std::fill_n(m_initializedMask[blockIndex].get() + offset, bytesToWrite, true);

        bytesWritten += bytesToWrite;
        ++blockIndex;
        offset = 0;
    }

    return BusStatus::Ok;
}

BusStatus Ram::readBytesWithUninitBehavior(
    size_t blockIndex,
    size_t offset,
    uint8_t* destBuf,
    size_t bytesToRead
    )
{
    // if block exists, read actual data
    if (m_blocks[blockIndex])
    {
        uint8_t* blockBytes = m_blocks[blockIndex].get();
        std::memcpy(destBuf, &blockBytes[offset], bytesToRead);
        return BusStatus::Ok;
    }

    // else apply behavior for uninitialized memory
    switch (m_ramBehavior)
    {
    case RamUninitBehavior::ZeroFill:
        std::fill_n(destBuf, bytesToRead, 0);
        return BusStatus::Ok;
    }

    return BusStatus::AccessFault;
}

void Ram::allocateBlock(size_t blockIndex)
{
    if (m_blocks[blockIndex])
        return;

    m_blocks[blockIndex] = std::make_unique<uint8_t[]>(m_blockSize);
    m_initializedMask[blockIndex] = std::make_unique<bool[]>(m_blockSize);

    switch (m_ramBehavior)
    {
    case RamUninitBehavior::ZeroFill:
        std::fill_n(m_blocks[blockIndex].get(), m_blockSize, 0);
        break;
    }
}

} // namespace Core
