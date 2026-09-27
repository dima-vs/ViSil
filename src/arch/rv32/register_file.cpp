#include "register_file.h"
#include <algorithm>
#include <cassert>

namespace RV32
{

uint32_t RegisterFile::readGPR(uint8_t index) const
{
    assert(index < 32 && "RISC-V GPR index must be between 0 and 31");
    if (index == 0)
        return 0;

    return m_gprArray[index - 1];
}

void RegisterFile::writeGPR(uint8_t index, uint32_t val)
{
    assert(index < 32 && "RISC-V GPR index must be between 0 and 31");
    // ignore writes to the x0 register
    if (index == 0)
        return;

    m_gprArray[index - 1] = val;
}

void RegisterFile::resetAllGPR()
{
    std::fill(m_gprArray.begin(), m_gprArray.end(), 0);
}

void RegisterFile::resetGPR(uint8_t index)
{
    assert(index < 32 && "RISC-V GPR index must be between 0 and 31");
    if (index == 0)
        return;

    m_gprArray[index - 1] = 0;
}

uint32_t RegisterFile::getPC() const
{
    return m_pc;
}

void RegisterFile::setPC(uint32_t val)
{
    m_pc = val;
}

void RegisterFile::advancePC(int32_t offset)
{
    m_pc += offset;
}


} // namespace RV32
