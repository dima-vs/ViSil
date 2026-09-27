#pragma once
#include <cstdint>
#include <array>
#include <string_view>

namespace RV32
{

class RegisterFile
{
private:
    // General purpose registers.
    // No need to store x0 value, because it's always zero
    std::array<uint32_t, 31> m_gprArray{};
    uint32_t m_pc{0};
public:
    uint32_t readGPR(uint8_t index) const;
    void writeGPR(uint8_t index, uint32_t val);
    void resetAllGPR();
    void resetGPR(uint8_t index);

    uint32_t getPC() const;
    void setPC(uint32_t val);
    void advancePC(int32_t offset=4);

    static constexpr std::string_view getAbiName(uint8_t index)
    {
        constexpr std::array<std::string_view, 32> abiNames = {
            "zero", "ra", "sp",  "gp",  "tp", "t0", "t1", "t2",
            "s0",   "s1", "a0",  "a1",  "a2", "a3", "a4", "a5",
            "a6",   "a7", "s2",  "s3",  "s4", "s5", "s6", "s7",
            "s8",   "s9", "s10", "s11", "t3", "t4", "t5", "t6"
        };
        return (index < 32) ? abiNames[index] : "unknown";
    }

    static constexpr std::string_view getArchitecturalName(uint8_t index)
    {
        constexpr std::array<std::string_view, 32> archNames = {
            "x0", "x1", "x2",  "x3",  "x4", "x5", "x6", "x7",
            "x8",   "x9", "x10",  "x11",  "x12", "x13", "x14", "x15",
            "x16",   "x17", "x18",  "x19",  "x20", "x21", "x22", "x23",
            "x24",   "x25", "x26", "x27", "x28", "x29", "x30", "x31"
        };
        return (index < 32) ? archNames[index] : "unknown";
    }};

} // namespace RV32
