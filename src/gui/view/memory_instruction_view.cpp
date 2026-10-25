#include "memory_instruction_view.h"
#include "memory_instruction_delegate.h"
#include "memory_instruction_types.h"
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

    connect(this, &QTableView::clicked,
            this, &MemoryInstructionView::handleClick);
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

void MemoryInstructionView::handleClick(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    if (index.column() != static_cast<int>(Gui::MemInstrColumn::Breakpoint))
        return;

    uint64_t addr = index.data(static_cast<int>(Gui::MemInstrRole::AddressRole)).toULongLong();
    emit breakpointToggled(addr, index);
}

MemoryInstructionView::~MemoryInstructionView() {}


} // namespace Gui
