#include "system_bus.h"

namespace Core
{

void SystemBus::mapDevice(uint64_t baseAddress, uint64_t size, IBusDevice* device)
{
    m_devices.push_back({baseAddress, size, device});
}

BusStatus SystemBus::readBlock(uint64_t addr, uint8_t* buf, size_t size)
{
    for (const auto& entry : m_devices)
    {
        if (addr >= entry.baseAddress && addr < entry.baseAddress + entry.size)
        {
            if (addr + size > entry.baseAddress + entry.size)
            {
                return BusStatus::AccessFault;
            }

            uint64_t localAddr = addr - entry.baseAddress;
            return entry.device->readBlock(localAddr, buf, size);
        }
    }

    return BusStatus::UnmappedAddress;
}

BusStatus SystemBus::writeBlock(uint64_t addr, const uint8_t* buf, size_t size)
{
    for (const auto& entry : m_devices)
    {
        if (addr >= entry.baseAddress && addr < entry.baseAddress + entry.size)
        {
            if (addr + size > entry.baseAddress + entry.size)
            {
                return BusStatus::AccessFault;
            }

            uint64_t localAddr = addr - entry.baseAddress;
            return entry.device->writeBlock(localAddr, buf, size);
        }
    }

    return BusStatus::UnmappedAddress;
}

} // namespace Core
