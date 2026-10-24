#include "memory_instruction_view.h"
#include "memory_instruction_delegate.h"
#include <QHeaderView>

namespace Gui
{

MemoryInstructionView::MemoryInstructionView(QWidget *parent) :
    QTableView(parent)
{
    setMouseTracking(true);
    setItemDelegate(new MemoryInstructionDelegate(this));

    verticalHeader()->hide();
    horizontalHeader()->hide();

    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(24);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    setShowGrid(false);
}

void MemoryInstructionView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QTableView::mousePressEvent(event);
}

void MemoryInstructionView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);

    if (model)
    {
        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
        horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

        setColumnWidth(0, 20);
        setColumnWidth(1, 100);
    }
}


MemoryInstructionView::~MemoryInstructionView() {}


} // namespace Gui
