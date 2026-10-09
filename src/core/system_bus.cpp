#include "system_bus.h"
#include <limits>
#include <algorithm>

namespace Core
{

SystemBus::SystemBus(
    Endianness endianness,
    uint64_t fixedSize
    ) : IBusDevice(endianness), m_fixedSize(fixedSize) {}

bool SystemBus::mapDevice(uint64_t baseAddress, IBusDevice* device)
{
    if (device == nullptr)
        return false;

    uint64_t size = device->getSize();
    if (size == 0)
        return false;

    if (baseAddress > std::numeric_limits<uint64_t>::max() - size)
        return false;

    uint64_t endAddress = baseAddress + size;
    for (const auto& entry : m_deviceEntries)
    {
        uint64_t entryEndAddress = entry.baseAddress + entry.size;

        // if device address ranges overlap on the bus
        if ((baseAddress < entryEndAddress) &&
            (endAddress > entry.baseAddress))
        {
            return false;
        }
    }

    m_deviceEntries.push_back({baseAddress, size, device});

    if (m_fixedSize == 0)
        m_maxDynamicSize = std::max(m_maxDynamicSize, baseAddress + size);

    return true;
}

BusStatus SystemBus::readBlock(uint64_t addr, uint8_t* buf, size_t size)
{
    uint64_t localAddr;
    auto [dev, status] = routeAccess(addr, size, localAddr);
    return dev ? dev->readBlock(localAddr, buf, size) : status;
}

BusStatus SystemBus::writeBlock(uint64_t addr, const uint8_t* buf, size_t size)
{
    uint64_t localAddr;
    auto [dev, status] = routeAccess(addr, size, localAddr);
    return dev ? dev->writeBlock(localAddr, buf, size) : status;
}

BusStatus SystemBus::read8(uint64_t addr, uint8_t& outVal)
{
    uint64_t localAddr;
    auto [dev, status] = routeAccess(addr, 1, localAddr);
    return dev ? dev->read8(localAddr, outVal) : status;
}

BusStatus SystemBus::write8(uint64_t addr, uint8_t val)
{
    uint64_t localAddr;
    auto [dev, status] = routeAccess(addr, 1, localAddr);
    return dev ? dev->write8(localAddr, val) : status;
}

BusStatus SystemBus::read16(uint64_t addr, uint16_t& outVal)
{
    uint64_t localAddr;
    auto [dev, status] = routeAccess(addr, 2, localAddr);
    return dev ? dev->read16(localAddr, outVal) : status;
}

BusStatus SystemBus::write16(uint64_t addr, uint16_t val)
{
    uint64_t localAddr;
    auto [dev, status] = routeAccess(addr, 2, localAddr);
    return dev ? dev->write16(localAddr, val) : status;
}

BusStatus SystemBus::read32(uint64_t addr, uint32_t& outVal)
{
    uint64_t localAddr;
    auto [dev, status] = routeAccess(addr, 4, localAddr);
    return dev ? dev->read32(localAddr, outVal) : status;
}

BusStatus SystemBus::write32(uint64_t addr, uint32_t val)
{
    uint64_t localAddr;
    auto [dev, status] = routeAccess(addr, 4, localAddr);
    return dev ? dev->write32(localAddr, val) : status;
}

BusStatus SystemBus::read64(uint64_t addr, uint64_t& outVal)
{
    uint64_t localAddr;
    auto [dev, status] = routeAccess(addr, 8, localAddr);
    return dev ? dev->read64(localAddr, outVal) : status;
}

BusStatus SystemBus::write64(uint64_t addr, uint64_t val)
{
    uint64_t localAddr;
    auto [dev, status] = routeAccess(addr, 8, localAddr);
    return dev ? dev->write64(localAddr, val) : status;
}

std::pair<IBusDevice*, BusStatus> SystemBus::routeAccess(
    uint64_t addr,
    uint64_t size,
    uint64_t& localAddr
    ) const
{
    for (const auto& entry : m_deviceEntries)
    {
        if (addr >= entry.baseAddress && addr < entry.baseAddress + entry.size)
        {
            uint64_t remainingSpace = entry.size - (addr - entry.baseAddress);

            if (size > remainingSpace)
                return {nullptr, BusStatus::AccessFault};

            localAddr = addr - entry.baseAddress;
            return {entry.device, BusStatus::Ok};
        }
    }
    return {nullptr, BusStatus::UnmappedAddress};
}

uint64_t SystemBus::getSize() const
{
    return (m_fixedSize > 0) ? m_fixedSize : m_maxDynamicSize;
}

} // namespace Core
