#include "uart.h"

namespace Devices
{

Uart::Uart(Core::Endianness endianness) :
    Core::IBusDevice(endianness), m_size(1) {}

void Uart::setDataReceivedCallback(DataReceivedCallback cb)
{
    m_onDataReceived = cb;
}

Core::BusStatus Uart::write8(uint64_t addr, uint8_t val)
{
    if (addr == 0x0)
    {
        if (m_onDataReceived)
            m_onDataReceived(val);
        return Core::BusStatus::Ok;
    }
    return Core::BusStatus::UnmappedAddress;
}

Core::BusStatus Uart::write32(uint64_t addr, uint32_t val)
{
    return write8(addr, static_cast<uint8_t>(val & 0xFF));
}

Core::BusStatus Uart::readBlock(uint64_t, uint8_t*, size_t)
{
    return Core::BusStatus::AccessFault;
}

Core::BusStatus Uart::writeBlock(uint64_t, const uint8_t*, size_t)
{
    return Core::BusStatus::AccessFault;
}

uint64_t Uart::getSize() const
{
    return m_size;
}

} // namespace Devices
