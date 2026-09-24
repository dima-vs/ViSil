#pragma once
#include "bus_device_interface.h"
#include <cstdint>

namespace Core
{

class BusChainNode : public IBusDevice
{
protected:
    IBusDevice* m_next = nullptr;

public:
    explicit BusChainNode(IBusDevice* next_device) : m_next(next_device) {}
    virtual ~BusChainNode() = default;

    BusStatus readBlock(uint64_t addr, uint8_t* buf, size_t size) override
    {
        if (m_next)
            return m_next->readBlock(addr, buf, size);
        return BusStatus::AccessFault;
    }

    BusStatus writeBlock(uint64_t addr, const uint8_t* buf, size_t size) override
    {
        if (m_next)
            return m_next->writeBlock(addr, buf, size);
        return BusStatus::AccessFault;
    }
};

} // namespace Core
