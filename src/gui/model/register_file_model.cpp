#include "register_file_model.h"
#include "core/common.h"

#include <cstdint>
#include <QString>

namespace Gui
{

RegisterFileModel::RegisterFileModel(
    const Core::ICpu& cpu,
    QObject *parent
    ) : QAbstractTableModel(parent),
    m_cpu(cpu), m_columns(2),
    m_pcCellsSpan(m_columns)
{
    // +1 for PC
    m_registerFormats = QList<RegNumericFormat>(
        m_cpu.getRegisterCount() + 1,
        RegNumericFormat::Hex
        );
}

int RegisterFileModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    // PC occupies [columnCount] cells
    int totalCells = m_cpu.getRegisterCount() + m_pcCellsSpan;

    // round up the value
    return (totalCells + m_columns - 1) / m_columns;
}

int RegisterFileModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_columns;
}

void RegisterFileModel::cycleFormat(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    int regId = getRegisterIdFromIndex(index);
    if (regId < 0 || regId >= static_cast<int>(m_registerFormats.size()))
        return;

    uint64_t rawVal = getRegisterRawValue(regId);
    uint16_t wordWidth = static_cast<uint16_t>(m_cpu.getCpuInfo().wordWidth);

    m_registerFormats[regId] = getNextRegisterFormat(
        m_registerFormats[regId],
        rawVal,
        wordWidth
        );

    emit dataChanged(index, index, {Qt::DisplayRole, Qt::ToolTipRole, Qt::UserRole});
}

RegNumericFormat RegisterFileModel::getNextRegisterFormat(
    RegNumericFormat current,
    uint64_t rawValue,
    uint16_t wordWidth
    )
{
    switch (current)
    {
    case RegNumericFormat::Hex:
        return RegNumericFormat::SignedDec;

    case RegNumericFormat::SignedDec:
    {
        if (wordWidth == 0 || wordWidth > 64)
            return RegNumericFormat::Hex;

        uint64_t signBitMask = static_cast<uint64_t>(1) << (wordWidth - 1);
        bool isNegative = (rawValue & signBitMask) != 0;

        return isNegative ? RegNumericFormat::UnsignedDec : RegNumericFormat::Hex;
    }

    case RegNumericFormat::UnsignedDec:
        return RegNumericFormat::Hex;
    }

    return RegNumericFormat::Hex;
}

QVariant RegisterFileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int regId = getRegisterIdFromIndex(index);

    if (regId < 0)
        return QVariant();

    if (static_cast<RegisterFileRole>(role) == RegisterFileRole::RegisterNameRole)
    {
        return getRegisterName(regId);
    }

    if (static_cast<RegisterFileRole>(role) == RegisterFileRole::RegisterValueRole)
    {
        uint64_t rawValue = getRegisterRawValue(regId);
        return getFormattedValue(regId, rawValue);
    }

    if (static_cast<RegisterFileRole>(role) == RegisterFileRole::ValuePrefixRole)
    {
        return getValuePrefix(regId);
    }

    if (role == Qt::ToolTipRole)
    {
        RegNumericFormat fmt = m_registerFormats[regId];
        if (fmt == RegNumericFormat::Hex)
            return "Hex";
        if (fmt == RegNumericFormat::SignedDec)
            return "Signed Dec";
        if (fmt == RegNumericFormat::UnsignedDec)
            return "Unsigned Dec";
    }

    return QVariant();
}

QSize RegisterFileModel::span(const QModelIndex &index) const
{
    if ((index.row() == 0) && (index.column() == 0))
        return QSize(m_pcCellsSpan, 1);

    return QSize(1, 1);
}

int RegisterFileModel::getRegisterIdFromIndex(const QModelIndex &index) const
{
    if (index.row() == 0)
    {
        return (index.column() == 0) ? 0 : -1;
    }

    int regIndex = index.row() * m_columns + index.column() - m_pcCellsSpan;
    return regIndex + 1;
}

QString RegisterFileModel::getRegisterName(int regId) const
{
    if (regId == 0)
        return "PC";

    return QString(m_cpu.getRegisterName(regId - 1).data());
}

uint64_t RegisterFileModel::getRegisterRawValue(int regId) const
{
    if (regId == 0)
        return m_cpu.getPc();

    return m_cpu.getRegister(regId - 1);
}

QString RegisterFileModel::getValuePrefix(int regId) const
{
    RegNumericFormat format = m_registerFormats.value(regId, RegNumericFormat::Hex);

    if (format == RegNumericFormat::Hex)
        return "0x";

    return "";
}

QString RegisterFileModel::getFormattedValue(int regId, uint64_t rawValue) const
{
    RegNumericFormat format = m_registerFormats.value(regId, RegNumericFormat::Hex);

    Core::BitWidth wordWidth = m_cpu.getCpuInfo().wordWidth;
    uint16_t widthBits = static_cast<uint16_t>(wordWidth);

    switch (format)
    {
    case RegNumericFormat::Hex:
    {
        int hexChars = widthBits / 4;
        return QString("%1").arg(rawValue, hexChars, 16, QChar('0'))
                .toUpper();
    }

    case RegNumericFormat::UnsignedDec:
    {
        return QString::number(rawValue);
    }

    case RegNumericFormat::SignedDec:
    {
        int64_t signedVal = 0;

        switch (wordWidth)
        {
        case Core::BitWidth::Bits8:
            signedVal = static_cast<int8_t>(rawValue);
            break;
        case Core::BitWidth::Bits16:
            signedVal = static_cast<int16_t>(rawValue);
            break;
        case Core::BitWidth::Bits32:
            signedVal = static_cast<int32_t>(rawValue);
            break;
        case Core::BitWidth::Bits64:
            signedVal = static_cast<int64_t>(rawValue);
            break;
        default:
            return "ERR";
        }

        return QString::number(signedVal);
    }
    }

    return "ERR";
}

} // namespace Gui
