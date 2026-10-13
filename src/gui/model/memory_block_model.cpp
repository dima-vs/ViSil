#include <QVariant>
#include <QString>
#include <QChar>
#include "memory_block_model.h"

void MemoryBlockModel::setGridColumns(int cols)
{
    if (cols < 1 || m_gridColumns == cols)
        return;

    beginResetModel();
    m_gridColumns = cols;
    endResetModel();
}

int MemoryBlockModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    uint64_t totalSize = m_device.getSize();
    if (totalSize == 0)
        return 0;

    // round up the value
    uint64_t totalBlocks = (totalSize + m_displayBlockSize - 1) / m_displayBlockSize;
    return (totalBlocks + m_gridColumns - 1) / m_gridColumns;
}

int MemoryBlockModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_gridColumns;
}

QVariant MemoryBlockModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    uint64_t totalSize = m_device.getSize();
    uint64_t totalBlocks = (totalSize + m_displayBlockSize - 1) / m_displayBlockSize;
    uint64_t blockIndex = (index.row() * m_gridColumns) + index.column();

    if (blockIndex >= totalBlocks)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        uint64_t addr = blockIndex * m_displayBlockSize;
        uint64_t absoluteAddr = m_baseAddress + addr;

        return QString("0x") + QString("%1")
                    .arg(absoluteAddr, 8, 16, QChar('0'))
                    .toUpper();
    }

    return QVariant();
}
