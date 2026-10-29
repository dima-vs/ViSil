#pragma once

#include <QObject>
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QSize>

#include <QList>
#include <cstdint>
#include "core/cpu_interface.h"
#include "gui/common/register_file_types.h"

namespace Gui
{

class RegisterFileModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    int m_columns;
    int m_pcCellsSpan;
    QList<RegNumericFormat> m_registerFormats;
    const Core::ICpu& m_cpu;

    // Returns 0 for PC, 1..N for reg0..regN.
    // Returns -1 for incorrect index
    int getRegisterIdFromIndex(const QModelIndex &index) const;

    QString getRegisterName(int regId) const;
    uint64_t getRegisterRawValue(int regId) const;
    QString getFormattedValue(int regId, uint64_t rawValue) const;
    QString getValuePrefix(int regId) const;


    RegNumericFormat getNextRegisterFormat(
        RegNumericFormat current,
        uint64_t rawValue,
        uint16_t wordWidth
        );
public:
    RegisterFileModel(
        const Core::ICpu& cpu,
        QObject *parent = nullptr
        );

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    QSize span(const QModelIndex &index) const override;

public slots:
    void cycleFormat(const QModelIndex &index);
};

} // namespace Gui
