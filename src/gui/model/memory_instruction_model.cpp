#include "memory_instruction_model.h"
#include "gui/common/memory_instruction_types.h"
#include "core/endian_utils.h"

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

    if (customRole == MemInstrRole::CurrentPcRole)
    {
        return (effectiveAddr == m_currentPc);
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
            uint8_t buffer[8] = {0};

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
                replaceIllegalTokensWithBytes(instr, rawInstr);
            }
            else if (m_wordWidth == 4)
            {
                rawInstr = Core::Endian::read32(buffer, currentEndian);
                replaceIllegalTokensWithBytes(instr, rawInstr);
            }
            else if (m_wordWidth == 8)
            {
                rawInstr = Core::Endian::read64(buffer, currentEndian);
                replaceIllegalTokensWithBytes(instr, rawInstr);
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

    int hexDigits = m_wordWidth * 2;
    QString rawString = QString("0x") +
                        QString("%1").arg(rawBytes, hexDigits, 16, QChar('0'))
                                     .toUpper();

    instr.tokens.clear();
    Core::DisasmToken token;
    token.type = Core::TokenType::Incorrect;
    token.text = rawString.toLatin1();
    instr.tokens.push_back(token);
}

void MemoryInstructionModel::setCurrentPc(uint64_t pc)
{
    if (m_currentPc == pc)
        return;

    int oldRow = getRowByAddress(m_currentPc);
    m_currentPc = pc;

    int newRow = getRowByAddress(m_currentPc);

    if (oldRow >= 0)
        emit dataChanged(index(oldRow, 0), index(oldRow, columnCount() - 1));

    if (newRow >= 0)
        emit dataChanged(index(newRow, 0), index(newRow, columnCount() - 1));
}

int MemoryInstructionModel::getRowByAddress(uint64_t address) const
{
    if (m_blockSize == 0 || m_wordWidth == 0)
        return -1;

    if (address >= m_blockBaseAddress && address < (m_blockBaseAddress + m_blockSize))
    {
        return (address - m_blockBaseAddress) / m_wordWidth;
    }
    return -1;
}

} // namespace Gui
