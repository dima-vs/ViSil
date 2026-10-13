#pragma once

#include <QObject>
#include <QAbstractTableModel>
#include <QModelIndex>

#include <cstdint>
#include "ram.h"

class MemoryBlockModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    const Devices::Ram &m_ram;
    int m_gridColumns = 4;
public slots:
    void setGridColumns(int cols);
public:
    MemoryBlockModel(
        const Devices::Ram &ram,
        QObject *parent = nullptr
        ) : QAbstractTableModel(parent),
        m_ram(ram) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
};
