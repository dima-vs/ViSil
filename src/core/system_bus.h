#pragma once
#include "bus_device_interface.h"
#include <vector>
#include <memory>

namespace Core
{

class SystemBus : public IBusDevice
{
private:
    struct MemoryMapEntry
    {
        uint64_t baseAddress;
        uint64_t size;
        IBusDevice* device;
    };

    std::vector<MemoryMapEntry> m_devices;

public:
    SystemBus(Primitives::Endianness endianness = Primitives::Endianness::Little)
        : IBusDevice(endianness) {}

    void mapDevice(uint64_t baseAddress, uint64_t size, IBusDevice* device);
    BusStatus readBlock(uint64_t addr, uint8_t* buf, size_t size) override;
    BusStatus writeBlock(uint64_t addr, const uint8_t* buf, size_t size) override;
};

} // namespace Core
