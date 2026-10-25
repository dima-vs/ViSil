#pragma once

#include <QTableView>
#include <QMouseEvent>
#include <QModelIndex>

namespace Gui
{

class MemoryInstructionView : public QTableView
{
    Q_OBJECT

public:
    MemoryInstructionView(QWidget *parent = nullptr);

    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void setModel(QAbstractItemModel *model) override;

    ~MemoryInstructionView();
signals:
    void breakpointToggled(uint64_t addr, const QModelIndex &index);
private slots:
    void handleClick(const QModelIndex &index);
};

} // namespace Gui
