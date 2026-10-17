#pragma once

#include <QObject>
#include <QVariant>
#include <QAbstractTableModel>
#include <QModelIndex>

#include <cstdint>
#include "bus_device_interface.h"
#include "common.h"
#include "memory_word_types.h"

class MemoryWordModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    enum class NumericFormat
    {
        Hex,
        UnsignedDec,
        SignedDec
    };

    Core::Endianness m_wordEndianness = Core::Endianness::Little;
    // values in bytes
    int m_displayWordWidth = 4;
    int m_addressWidthBytes = 4;

    NumericFormat m_numericFormat = NumericFormat::Hex;
    const Core::IBusDevice &m_device;
    uint64_t m_blockBaseAddress = 0;
    uint64_t m_blockSize = 0;
public:
    MemoryWordModel(
        const Core::IBusDevice &device,
        QObject *parent = nullptr
        );

    void inspectBlock(uint64_t baseAddress, uint64_t blockSize);

    void setDisplayWordWidth(Core::BitWidth bitWidth);
    void setAddressWidth(Core::BitWidth bitWidth);
    void setWordEndianness(Core::Endianness endianness);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public slots:
    void onHeaderSectionClicked(int logicalIndex);
    void cycleNumericFormat();
};
