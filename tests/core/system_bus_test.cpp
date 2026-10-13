#include <gtest/gtest.h>
#include <cstdint>
#include <limits>
#include <vector>

#include "system_bus.h"
#include "bus_device_interface.h"

using namespace Core;

namespace
{

enum class LastCall
{
    None,
    ReadBlock,
    WriteBlock,
    Read8,
    Write8,
    Read16,
    Write16,
    Read32,
    Write32,
    Read64,
    Write64,
    PeekBlock,
    Peek8,
    Peek16,
    Peek32,
    Peek64
};

class MockBusDevice : public IBusDevice
{
public:
    uint64_t deviceSize = 0x1000;
    mutable LastCall lastCall = LastCall::None;
    mutable uint64_t lastLocalAddr = 0;
    mutable size_t lastSize = 0;

    uint8_t  val8  = 0x42;
    uint16_t val16 = 0x1234;
    uint32_t val32 = 0xCAFEBABE;
    uint64_t val64 = 0x0123456789ABCDEF;

    MockBusDevice(uint64_t size = 0x1000, Endianness endian = Endianness::Little)
        : IBusDevice(endian), deviceSize(size) {}

    uint64_t getSize() const override { return deviceSize; }

    BusStatus readBlock(uint64_t addr, uint8_t* buf, size_t size) override
    {
        lastCall = LastCall::ReadBlock;
        lastLocalAddr = addr;
        lastSize = size;

        if (buf && size > 0)
            buf[0] = 0xAA;

        return BusStatus::Ok;
    }

    BusStatus writeBlock(uint64_t addr, const uint8_t*, size_t size) override
    {
        lastCall = LastCall::WriteBlock;
        lastLocalAddr = addr;
        lastSize = size;

        return BusStatus::Ok;
    }

    BusStatus read8(uint64_t addr, uint8_t& outVal) override
    {
        lastCall = LastCall::Read8;
        lastLocalAddr = addr;
        outVal = val8;

        return BusStatus::Ok;
    }

    BusStatus write8(uint64_t addr, uint8_t val) override
    {
        lastCall = LastCall::Write8;
        lastLocalAddr = addr;
        val8 = val;

        return BusStatus::Ok;
    }

    BusStatus read16(uint64_t addr, uint16_t& outVal) override
    {
        lastCall = LastCall::Read16;
        lastLocalAddr = addr;
        outVal = val16;

        return BusStatus::Ok;
    }

    BusStatus write16(uint64_t addr, uint16_t val) override
    {
        lastCall = LastCall::Write16;
        lastLocalAddr = addr;
        val16 = val;

        return BusStatus::Ok;
    }

    BusStatus read32(uint64_t addr, uint32_t& outVal) override
    {
        lastCall = LastCall::Read32;
        lastLocalAddr = addr;
        outVal = val32;

        return BusStatus::Ok;
    }

    BusStatus write32(uint64_t addr, uint32_t val) override
    {
        lastCall = LastCall::Write32;
        lastLocalAddr = addr;
        val32 = val;

        return BusStatus::Ok;
    }

    BusStatus read64(uint64_t addr, uint64_t& outVal) override
    {
        lastCall = LastCall::Read64;
        lastLocalAddr = addr;
        outVal = val64;

        return BusStatus::Ok;
    }

    BusStatus write64(uint64_t addr, uint64_t val) override
    {
        lastCall = LastCall::Write64;
        lastLocalAddr = addr;
        val64 = val;

        return BusStatus::Ok;
    }

    BusStatus peekBlock(uint64_t addr, uint8_t* buf, size_t size) const override
    {
        lastCall = LastCall::PeekBlock;
        lastLocalAddr = addr;
        lastSize = size;

        if (buf && size > 0)
            buf[0] = 0x55;

        return BusStatus::Ok;
    }

    BusStatus peek8(uint64_t addr, uint8_t& outVal) const override
    {
        lastCall = LastCall::Peek8;
        lastLocalAddr = addr;
        outVal = val8;

        return BusStatus::Ok;
    }

    BusStatus peek16(uint64_t addr, uint16_t& outVal) const override
    {
        lastCall = LastCall::Peek16;
        lastLocalAddr = addr;
        outVal = val16;

        return BusStatus::Ok;
    }

    BusStatus peek32(uint64_t addr, uint32_t& outVal) const override
    {
        lastCall = LastCall::Peek32;
        lastLocalAddr = addr;
        outVal = val32;

        return BusStatus::Ok;
    }

    BusStatus peek64(uint64_t addr, uint64_t& outVal) const override
    {
        lastCall = LastCall::Peek64;
        lastLocalAddr = addr;
        outVal = val64;

        return BusStatus::Ok;
    }
};

} // namespace


TEST(SystemBusTest, MapDeviceSuccessAndDynamicSize)
{
    SystemBus bus;
    MockBusDevice dev1(0x1000); // 4 KB
    MockBusDevice dev2(0x2000); // 8 KB

    EXPECT_TRUE(bus.mapDevice(0x0000, &dev1));
    EXPECT_EQ(bus.getSize(), 0x1000);

    EXPECT_TRUE(bus.mapDevice(0x1000, &dev2));
    EXPECT_EQ(bus.getSize(), 0x3000);
}

TEST(SystemBusTest, RejectNullptrAndZeroSize)
{
    SystemBus bus;
    EXPECT_FALSE(bus.mapDevice(0x1000, nullptr));

    MockBusDevice zeroDev(0);
    EXPECT_FALSE(bus.mapDevice(0x1000, &zeroDev));
}

TEST(SystemBusTest, DetectOverlapCollisions)
{
    SystemBus bus;
    MockBusDevice baseDev(0x1000); // [0x1000 .. 0x2000)
    EXPECT_TRUE(bus.mapDevice(0x1000, &baseDev));

    MockBusDevice exactMatch(0x1000);
    EXPECT_FALSE(bus.mapDevice(0x1000, &exactMatch));

    MockBusDevice partialLeft(0x800); // [0x0A00 .. 0x1200)
    EXPECT_FALSE(bus.mapDevice(0x0A00, &partialLeft));

    MockBusDevice partialRight(0x800); // [0x1E00 .. 0x2600)
    EXPECT_FALSE(bus.mapDevice(0x1E00, &partialRight));

    MockBusDevice fullyInside(0x200); // [0x1200 .. 0x1400)
    EXPECT_FALSE(bus.mapDevice(0x1200, &fullyInside));

    // end-to-end map has to be successful.
    MockBusDevice beforeDev(0x1000); // [0x0000 .. 0x1000)
    EXPECT_TRUE(bus.mapDevice(0x0000, &beforeDev));

    MockBusDevice afterDev(0x1000); // [0x2000 .. 0x3000)
    EXPECT_TRUE(bus.mapDevice(0x2000, &afterDev));
}

TEST(SystemBusTest, PreventAddressOverflowOnMapping)
{
    SystemBus bus;
    MockBusDevice dev(0x1000);

    // baseAddress + size leads to uint64_t overflow
    uint64_t overflowBase = std::numeric_limits<uint64_t>::max() - 0x100;
    EXPECT_FALSE(bus.mapDevice(overflowBase, &dev));
}

TEST(SystemBusTest, FixedSizeOverridesDynamicSize)
{
    SystemBus bus(Endianness::Little, 0x100000); // 1 MB
    MockBusDevice dev(0x1000);

    EXPECT_TRUE(bus.mapDevice(0x0, &dev));
    EXPECT_EQ(bus.getSize(), 0x100000);
}

TEST(SystemBusTest, DirectRoutingPreservesTransactionTypes)
{
    SystemBus bus;
    MockBusDevice dev(0x1000);
    EXPECT_TRUE(bus.mapDevice(0x2000, &dev));

    // Write8
    EXPECT_EQ(bus.write8(0x2005, 0x77), BusStatus::Ok);
    EXPECT_EQ(dev.lastCall, LastCall::Write8);
    EXPECT_EQ(dev.lastLocalAddr, 0x5);
    EXPECT_EQ(dev.val8, 0x77);

    // Write32
    EXPECT_EQ(bus.write32(0x2020, 0x11223344), BusStatus::Ok);
    EXPECT_EQ(dev.lastCall, LastCall::Write32);
    EXPECT_EQ(dev.lastLocalAddr, 0x20);
    EXPECT_EQ(dev.val32, 0x11223344);

    // Read16
    uint16_t out16 = 0;
    EXPECT_EQ(bus.read16(0x2010, out16), BusStatus::Ok);
    EXPECT_EQ(dev.lastCall, LastCall::Read16);
    EXPECT_EQ(dev.lastLocalAddr, 0x10);
    EXPECT_EQ(out16, dev.val16);

    // Read64
    uint64_t out64 = 0;
    EXPECT_EQ(bus.read64(0x2040, out64), BusStatus::Ok);
    EXPECT_EQ(dev.lastCall, LastCall::Read64);
    EXPECT_EQ(dev.lastLocalAddr, 0x40);
    EXPECT_EQ(out64, dev.val64);
}

TEST(SystemBusTest, RouteBlockOperations)
{
    SystemBus bus;
    MockBusDevice dev(0x1000);
    EXPECT_TRUE(bus.mapDevice(0x4000, &dev));

    uint8_t buffer[16] = {0};
    EXPECT_EQ(bus.readBlock(0x4080, buffer, sizeof(buffer)), BusStatus::Ok);
    EXPECT_EQ(dev.lastCall, LastCall::ReadBlock);
    EXPECT_EQ(dev.lastLocalAddr, 0x80);
    EXPECT_EQ(dev.lastSize, sizeof(buffer));

    EXPECT_EQ(bus.writeBlock(0x4090, buffer, sizeof(buffer)), BusStatus::Ok);
    EXPECT_EQ(dev.lastCall, LastCall::WriteBlock);
    EXPECT_EQ(dev.lastLocalAddr, 0x90);
    EXPECT_EQ(dev.lastSize, sizeof(buffer));
}

TEST(SystemBusTest, UnmappedAddress)
{
    SystemBus bus;
    MockBusDevice dev(0x1000);
    EXPECT_TRUE(bus.mapDevice(0x1000, &dev));

    uint32_t val = 0;
    // access below the mapped device address range
    EXPECT_EQ(bus.read32(0x0500, val), BusStatus::UnmappedAddress);
    // access above the mapped device address range
    EXPECT_EQ(bus.write32(0x3000, 0x123), BusStatus::UnmappedAddress);
}

TEST(SystemBusTest, AccessFaultOnDeviceBoundarySpill)
{
    SystemBus bus;
    MockBusDevice dev(0x100); // [0x1000 .. 0x1100), size 256 bytes
    EXPECT_TRUE(bus.mapDevice(0x1000, &dev));

    uint32_t val = 0;
    // address 0x10FE is in range, but 4 byte read goes above 0x1100
    EXPECT_EQ(bus.read32(0x10FE, val), BusStatus::AccessFault);

    uint8_t buf[10];
    EXPECT_EQ(bus.readBlock(0x10F8, buf, 10), BusStatus::AccessFault);
}

TEST(SystemBusTest, ConstPeekRouting)
{
    SystemBus bus;
    MockBusDevice dev(0x1000);
    EXPECT_TRUE(bus.mapDevice(0x8000, &dev));

    const SystemBus& constBus = bus;

    uint8_t val8 = 0;
    EXPECT_EQ(constBus.peek8(0x8001, val8), BusStatus::Ok);
    EXPECT_EQ(dev.lastCall, LastCall::Peek8);
    EXPECT_EQ(dev.lastLocalAddr, 0x1);
    EXPECT_EQ(val8, dev.val8);

    uint32_t val32 = 0;
    EXPECT_EQ(constBus.peek32(0x8010, val32), BusStatus::Ok);
    EXPECT_EQ(dev.lastCall, LastCall::Peek32);
    EXPECT_EQ(dev.lastLocalAddr, 0x10);
    EXPECT_EQ(val32, dev.val32);

    uint8_t blockBuf[8] = {0};
    EXPECT_EQ(constBus.peekBlock(0x8020, blockBuf, 8), BusStatus::Ok);
    EXPECT_EQ(dev.lastCall, LastCall::PeekBlock);
    EXPECT_EQ(dev.lastLocalAddr, 0x20);
    EXPECT_EQ(dev.lastSize, 8);
}
