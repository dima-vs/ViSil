#pragma once

#include <QTableView>
#include <QMouseEvent>
#include <QModelIndex>

#include "memory_instruction_delegate.h"
#include "inline_error_card.h"
#include <QResizeEvent>

namespace Gui
{

class MemoryInstructionView : public QTableView
{
    Q_OBJECT

private:
    MemoryInstructionDelegate* m_delegate = nullptr;
    InlineErrorCard* m_errorCard = nullptr;
    int m_errorRow = -1;
public:
    MemoryInstructionView(QWidget *parent = nullptr);

    void scrollToRow(int row);
    void setModel(QAbstractItemModel *model) override;

    void showErrorAtRow(int row, const QString& message);
    void hideError();

    ~MemoryInstructionView();
protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
signals:
    void breakpointToggled(uint64_t addr, const QModelIndex &index);
private slots:
    void updateOverlayPositions();

    void handleClick(const QModelIndex &index);
};

} // namespace Gui
