#pragma once

#include <QTableView>
#include <QModelIndex>

namespace Gui
{

class RegisterFileView : public QTableView
{
    Q_OBJECT

public:
    RegisterFileView(QWidget *parent = nullptr);
    ~RegisterFileView();

    void setModel(QAbstractItemModel *model) override;
signals:
    void numericFormatCycleRequested(const QModelIndex &index);
};

} // namespace Gui
