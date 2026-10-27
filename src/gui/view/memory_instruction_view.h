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

    void scrollToRow(int row);
    void setModel(QAbstractItemModel *model) override;

    ~MemoryInstructionView();
protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
signals:
    void breakpointToggled(uint64_t addr, const QModelIndex &index);
private slots:
    void handleClick(const QModelIndex &index);
};

} // namespace Gui
