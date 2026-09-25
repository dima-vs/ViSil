#include <gtest/gtest.h>
#include "ram.h"

using namespace Core;

TEST(RamTest, ReadWrite32LittleEndian)
{
    Ram ram(4, 4096); // 4 blocks, each 4 KB

    uint32_t writeVal = 0x12345678;
    EXPECT_EQ(ram.write32(0x100, writeVal), BusStatus::Ok);

    uint32_t readVal = 0;
    EXPECT_EQ(ram.read32(0x100, readVal), BusStatus::Ok);
    EXPECT_EQ(readVal, writeVal);

    // check phisical order (Little-Endian: 78 56 34 12)
    uint8_t rawBytes[4];
    EXPECT_EQ(ram.readBlock(0x100, rawBytes, 4), BusStatus::Ok);
    EXPECT_EQ(rawBytes[0], 0x78);
    EXPECT_EQ(rawBytes[1], 0x56);
    EXPECT_EQ(rawBytes[2], 0x34);
    EXPECT_EQ(rawBytes[3], 0x12);
}

TEST(RamTest, ReadWrite32BigEndian)
{
    Ram ram(4, 4096, // 4 blocks, each 4 KB
        RamUninitBehavior::ZeroFill,
        Primitives::Endianness::Big
        );

    uint32_t writeVal = 0x12345678;
    EXPECT_EQ(ram.write32(0x100, writeVal), BusStatus::Ok);

    uint32_t readVal = 0;
    EXPECT_EQ(ram.read32(0x100, readVal), BusStatus::Ok);
    EXPECT_EQ(readVal, writeVal);

    // check phisical order (Big-Endian: 12 34 56 78)
    uint8_t rawBytes[4];
    EXPECT_EQ(ram.readBlock(0x100, rawBytes, 4), BusStatus::Ok);
    EXPECT_EQ(rawBytes[0], 0x12);
    EXPECT_EQ(rawBytes[1], 0x34);
    EXPECT_EQ(rawBytes[2], 0x56);
    EXPECT_EQ(rawBytes[3], 0x78);
}

TEST(RamTest, LazyAllocationUninitializedRead)
{
    Ram ram(2, 4096, RamUninitBehavior::ZeroFill);

    uint32_t val = 0xFFFFFFFF;
    EXPECT_EQ(ram.read32(0x200, val), BusStatus::Ok);
    EXPECT_EQ(val, 0x00000000);
}

TEST(RamTest, CrossPageAccess)
{
    Ram ram(4, 64,
        RamUninitBehavior::ZeroFill,
        Primitives::Endianness::Big
        );
    uint32_t val = 0x87654321;

    // [ ---  PAGE_1 --- ] -> [ --- PAGE_2 --- ] -> ...
    // 0                63    64             127
    //               └─0x87654321─┘
    EXPECT_EQ(ram.write32(62, val), BusStatus::Ok);

    uint32_t readVal = 0;
    EXPECT_EQ(ram.read32(62, readVal), BusStatus::Ok);
    EXPECT_EQ(readVal, val);

    // check that endianness is preserved in cross page access
    // Big-Endian: 87 65 43 21
    uint8_t rawBytes[4];
    EXPECT_EQ(ram.readBlock(62, rawBytes, 4), BusStatus::Ok);
    EXPECT_EQ(rawBytes[0], 0x87);
    EXPECT_EQ(rawBytes[1], 0x65);
    EXPECT_EQ(rawBytes[2], 0x43);
    EXPECT_EQ(rawBytes[3], 0x21);
}

TEST(RamTest, OutOfBoundsAccess)
{
    Ram ram(2, 4096); // capacity: 8192 bytes (0x0000 - 0x1FFF)

    uint32_t dummy = 0;
    EXPECT_EQ(ram.read32(8192, dummy), BusStatus::UnmappedAddress);
    EXPECT_EQ(ram.write32(8192, 0x1234), BusStatus::UnmappedAddress);

    // try to read 4 bytes starting from 8190
    EXPECT_EQ(ram.read32(8190, dummy), BusStatus::UnmappedAddress);
}

TEST(RamTest, ReadWrite64LittleEndian)
{
    Ram ram(1, 4096);

    uint64_t val64 = 0x0123456789ABCDEF;
    EXPECT_EQ(ram.write64(0x40, val64), BusStatus::Ok);

    uint64_t readVal64 = 0;
    EXPECT_EQ(ram.read64(0x40, readVal64), BusStatus::Ok);
    EXPECT_EQ(readVal64, val64);
}

TEST(RamTest, ReadWrite16LittleEndian)
{
    Ram ram(1, 4096);

    uint16_t val16 = 0x01234;
    EXPECT_EQ(ram.write16(0x40, val16), BusStatus::Ok);

    uint16_t readVal16 = 0;
    EXPECT_EQ(ram.read16(0x40, readVal16), BusStatus::Ok);
    EXPECT_EQ(readVal16, val16);
}
