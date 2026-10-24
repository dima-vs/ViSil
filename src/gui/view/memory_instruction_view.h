#pragma once

#include <QTableView>
#include <QMouseEvent>

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
};

} // namespace Gui
