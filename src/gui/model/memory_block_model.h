#pragma once

#include <QObject>
#include <QAbstractTableModel>
#include <QModelIndex>

#include <cstdint>
#include "bus_device_interface.h"

namespace Gui
{

class MemoryBlockModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    const Core::IBusDevice &m_device;
    // Device base address.
    // Required to access the device on a system bus.
    uint64_t m_baseAddress;
    uint64_t m_displayBlockSize = 256;
    int m_gridColumns = 4;
public slots:
    void setGridColumns(int cols);
public:
    MemoryBlockModel(
        const Core::IBusDevice &device,
        uint64_t baseAddress = 0,
        QObject *parent = nullptr
        ) : QAbstractTableModel(parent),
        m_device(device),
        m_baseAddress(baseAddress) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
};

} // namespace Gui
