#include "memory_word_model.h"
#include "endian_utils.h"
#include <QChar>
#include <QString>
#include <vector>

MemoryWordModel::MemoryWordModel(
    const Core::IBusDevice &device,
    QObject *parent
    ) : QAbstractTableModel(parent), m_device(device),
        m_wordEndianness(device.getEndianness())
{

}

void MemoryWordModel::cycleNumericFormat()
{
    switch (m_numericFormat)
    {
    case NumericFormat::Hex:
        m_numericFormat = NumericFormat::UnsignedDec;
        break;
    case NumericFormat::UnsignedDec:
        m_numericFormat = NumericFormat::SignedDec;
        break;
    case NumericFormat::SignedDec:
        m_numericFormat = NumericFormat::Hex;
        break;
    }

    int colIdx = static_cast<int>(Column::NumericValue);
    emit headerDataChanged(Qt::Horizontal, colIdx, colIdx);

    if (rowCount() > 0)
    {
        emit dataChanged(
            index(0, colIdx),
            index(rowCount() - 1, colIdx),
            {Qt::DisplayRole}
            );
    }
}

void MemoryWordModel::setDisplayWordWidth(Core::BitWidth bitWidth)
{
    int newWidth = static_cast<uint16_t>(bitWidth) / 8;
    if (m_displayWordWidth == newWidth)
        return;

    beginResetModel();
    m_displayWordWidth = newWidth;
    endResetModel();
}

void MemoryWordModel::setAddressWidth(Core::BitWidth bitWidth)
{
    m_addressWidthBytes = static_cast<uint16_t>(bitWidth) / 8;
    emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
}

void MemoryWordModel::setWordEndianness(Core::Endianness endianness)
{
    if (m_wordEndianness == endianness)
        return;

    beginResetModel();
    m_wordEndianness = endianness;
    endResetModel();
}

void MemoryWordModel::inspectBlock(uint64_t baseAddress, uint64_t blockSize)
{
    beginResetModel();
    m_blockBaseAddress = baseAddress;
    m_blockSize = blockSize;
    endResetModel();
}

int MemoryWordModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || m_blockSize == 0)
        return 0;

    // round up the value
    return (m_blockSize + m_displayWordWidth - 1) / m_displayWordWidth;
}

int MemoryWordModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return static_cast<int>(Column::Count);
}

QVariant MemoryWordModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
        return QVariant();

    Column columnSec = static_cast<Column>(section);
    if (role == Qt::DisplayRole)
    {
        switch (static_cast<Column>(section))
        {
        case Column::Address:
            return "Address";

        case Column::RawBytes:
            return "Raw Bytes";

        case Column::NumericValue:
            switch (m_numericFormat)
            {
            case NumericFormat::Hex:
                return "Hex";
            case NumericFormat::SignedDec:
                return "Signed Dec";
            case NumericFormat::UnsignedDec:
                return "Unsigned Dec";
            }
            break;
        default:
            break;
        }
    }

    if (role == Qt::ToolTipRole)
    {
        if (columnSec == Column::RawBytes)
        {
            return (m_wordEndianness == Core::Endianness::Little)?
                       "Little Endian" : "Big Endian";
        }
    }

    return QVariant();
}

QVariant MemoryWordModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() ||
        index.column() >= static_cast<int>(Column::Count) ||
        index.column() < 0
        )
        return QVariant();

    uint64_t effectiveAddr = m_blockBaseAddress + (index.row() * m_displayWordWidth);

    if (role == Qt::UserRole)
    {
        CellMetadata meta;
        meta.column = static_cast<Column>(index.column());
        meta.endianness = m_wordEndianness;

        if (
            (meta.column == Column::Address) ||
            ((meta.column == Column::NumericValue) &&
            (m_numericFormat == NumericFormat::Hex))
            )
        {
            meta.prefix = "0x";
        }
        else
            meta.prefix = "";

        return QVariant::fromValue(meta);
    }

    if (role == Qt::DisplayRole)
    {
        Column col = static_cast<Column>(index.column());

        if (col == Column::Address)
        {
            int hexChars = m_addressWidthBytes * 2;
            return QString("%1")
                        .arg(effectiveAddr, hexChars, 16, QChar('0'))
                        .toUpper();
        }

        std::vector<uint8_t> buffer(m_displayWordWidth, 0);
        if (
            m_device.peekBlock(
                effectiveAddr,
                buffer.data(),
                m_displayWordWidth
                ) != Core::BusStatus::Ok
            )
        {
            return "?";
        }

        if (col == Column::RawBytes)
        {
            QString rawHex;
            for (int i = 0; i < m_displayWordWidth; ++i)
            {
                rawHex.append(QString("%1 ").arg(buffer[i], 2, 16, QChar('0')).toUpper());
            }
            return rawHex.trimmed();
        }

        if (col == Column::NumericValue)
        {
            uint64_t val = 0;
            switch (m_displayWordWidth)
            {
            case 1: val = buffer[0];
                break;
            case 2: val = Core::Endian::read16(buffer.data(), m_wordEndianness);
                break;
            case 4: val = Core::Endian::read32(buffer.data(), m_wordEndianness);
                break;
            case 8: val = Core::Endian::read64(buffer.data(), m_wordEndianness);
                break;
            default:
                return "ERR";
            }

            switch (m_numericFormat)
            {
            case NumericFormat::Hex:
            {
                int hexChars = m_displayWordWidth * 2;
                return QString("%1")
                            .arg(val, hexChars, 16, QChar('0'))
                            .toUpper();
            }
            case NumericFormat::UnsignedDec:
                return QString::number(val);

            case NumericFormat::SignedDec:
            {

                switch (m_displayWordWidth)
                {
                case 1:
                    return QString::number(static_cast<int8_t>(val));
                case 2:
                    return QString::number(static_cast<int16_t>(val));
                case 4:
                    return QString::number(static_cast<int32_t>(val));
                case 8:
                    return QString::number(static_cast<int64_t>(val));
                default:
                    return "ERR";
                }
            }
            }
        }
    }

    return QVariant();
}

void MemoryWordModel::onHeaderSectionClicked(int logicalIndex)
{
    if (logicalIndex < 0 || logicalIndex >= static_cast<int>(Column::Count))
        return;

    Column col = static_cast<Column>(logicalIndex);
    switch (col)
    {
    case Column::NumericValue:
        cycleNumericFormat();
        break;
    }
}
