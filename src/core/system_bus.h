#pragma once
#include "bus_device_interface.h"
#include <vector>
#include <memory>
#include <utility>

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

    std::vector<MemoryMapEntry> m_deviceEntries;
    uint64_t m_fixedSize;
    uint64_t m_maxDynamicSize = 0;

    std::pair<IBusDevice*, BusStatus> routeAccess(
        uint64_t addr,
        uint64_t size,
        uint64_t& localAddr
        ) const;
public:
    SystemBus(
        Endianness endianness = Endianness::Little,
        uint64_t fixedSize = 0
        );

    uint64_t getSize() const override;
    bool mapDevice(uint64_t baseAddress, IBusDevice* device);

    BusStatus readBlock(uint64_t addr, uint8_t* buf, size_t size) override;
    BusStatus writeBlock(uint64_t addr, const uint8_t* buf, size_t size) override;

    BusStatus read8(uint64_t addr, uint8_t& outVal) override;
    BusStatus write8(uint64_t addr, uint8_t val) override;
    BusStatus read16(uint64_t addr, uint16_t& outVal) override;
    BusStatus write16(uint64_t addr, uint16_t val) override;
    BusStatus read32(uint64_t addr, uint32_t& outVal) override;
    BusStatus write32(uint64_t addr, uint32_t val) override;
    BusStatus read64(uint64_t addr, uint64_t& outVal) override;
    BusStatus write64(uint64_t addr, uint64_t val) override;

    BusStatus peekBlock(uint64_t addr, uint8_t* buf, size_t size) const override;

    BusStatus peek8(uint64_t addr, uint8_t& outVal) const override;
    BusStatus peek16(uint64_t addr, uint16_t& outVal) const override;
    BusStatus peek32(uint64_t addr, uint32_t& outVal) const override;
    BusStatus peek64(uint64_t addr, uint64_t& outVal) const override;
};

} // namespace Core
