#pragma once
#include "bus_device_interface.h"
#include <cstdint>
#include <functional>

namespace Devices
{

class Uart : public Core::IBusDevice
{
private:
    using DataReceivedCallback = std::function<void(uint8_t)>;
    DataReceivedCallback m_onDataReceived;
    size_t m_size;
public:
    Uart(Core::Endianness endianness);

    void setDataReceivedCallback(DataReceivedCallback cb);

    uint64_t getSize() const override;

    Core::BusStatus write8(uint64_t addr, uint8_t val) override;
    Core::BusStatus write32(uint64_t addr, uint32_t val) override;

    Core::BusStatus readBlock(uint64_t, uint8_t*, size_t) override;
    Core::BusStatus writeBlock(uint64_t, const uint8_t*, size_t) override;
};

} // namespace Devices
