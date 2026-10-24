#include "memory_instruction_model.h"
#include "memory_instruction_types.h"
#include "endian_utils.h"

#include <QString>
#include <QVariant>

namespace Gui
{

MemoryInstructionModel::MemoryInstructionModel(
    const Core::SimulationRunner &simRunner,
    const Core::IBusDevice &device,
    const Core::IDisassembler &disassembler,
    QObject *parent
    ) : QAbstractTableModel(parent),
    m_simRunner(simRunner),
    m_device(device),
    m_disassembler(disassembler) {}

void MemoryInstructionModel::inspectBlock(uint64_t baseAddress, uint64_t blockSize)
{
    beginResetModel();
    m_blockBaseAddress = baseAddress;
    m_blockSize = blockSize;
    endResetModel();
}

int MemoryInstructionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || m_blockSize == 0)
        return 0;

    return (m_blockSize + m_wordWidth - 1) / m_wordWidth;
}

int MemoryInstructionModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return static_cast<int>(MemInstrColumn::Count);
}

QVariant MemoryInstructionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    uint64_t effectiveAddr = m_blockBaseAddress + (index.row() * m_wordWidth);
    MemInstrColumn col = static_cast<MemInstrColumn>(index.column());
    MemInstrRole customRole = static_cast<MemInstrRole>(role);

    if (role == Qt::DisplayRole)
    {
        if (col == MemInstrColumn::Breakpoint)
            return "";
        if (col == MemInstrColumn::Address)
            return QString("0x") + QString("%1").arg(effectiveAddr, 8, 16, QChar('0')).toUpper();
    }

    if (customRole == MemInstrRole::BreakPointRole)
    {
        if (col == MemInstrColumn::Breakpoint)
            return m_simRunner.hasBreakpoint(effectiveAddr);
    }

    if (customRole == MemInstrRole::AddressRole)
    {
        return effectiveAddr;
    }

    if (col == MemInstrColumn::Disassembly)
    {
        if (customRole == MemInstrRole::DisassembledInstrRole || role == Qt::DisplayRole)
        {
            uint8_t buffer[4] = {0};

            if (m_device.peekBlock(effectiveAddr, buffer, m_wordWidth) != Core::BusStatus::Ok)
            {
                return (role == Qt::DisplayRole) ? "Memory Error" : QVariant();
            }

            Core::DisassembledInstr instr = m_disassembler.disassemble(
                effectiveAddr, buffer, m_wordWidth);


            uint64_t rawInstr = 0;
            Core::Endianness currentEndian = m_device.getEndianness();

            if (m_wordWidth == 2)
            {
                rawInstr = Core::Endian::read16(buffer, currentEndian);
                replaceIllegalTokensWithBytes(instr, static_cast<uint32_t>(rawInstr));
            }
            else if (m_wordWidth == 4)
            {
                rawInstr = Core::Endian::read32(buffer, currentEndian);
                replaceIllegalTokensWithBytes(instr, static_cast<uint32_t>(rawInstr));
            }
            else if (m_wordWidth == 8)
            {
                rawInstr = Core::Endian::read64(buffer, currentEndian);
                replaceIllegalTokensWithBytes(instr, static_cast<uint32_t>(rawInstr));
            }

            if (customRole == MemInstrRole::DisassembledInstrRole)
            {
                return QVariant::fromValue(instr);
            }
            else // Qt::DisplayRole
            {
                return QString::fromStdString(instr.toString());
            }
        }
    }

    return QVariant();
}

void MemoryInstructionModel::replaceIllegalTokensWithBytes(
    Core::DisassembledInstr& instr,
    uint64_t rawBytes
    ) const
{
    if (!instr.isIllegal)
        return;

    QString rawString = QString("0x") + QString("%1")
                    .arg(rawBytes, 8, 16, QChar('0')).toUpper();

    instr.tokens.clear();
    Core::DisasmToken token;
    token.type = Core::TokenType::Incorrect;
    token.text = rawString.toLatin1();
    instr.tokens.push_back(token);
}

} // namespace Gui
