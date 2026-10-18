#include "memory_word_view.h"
#include "memory_word_delegate.h"
#include "memory_word_table_header.h"

#include <QHeaderView>

namespace Gui
{

MemoryWordView::MemoryWordView(QWidget *parent) :
    QTableView(parent)
{
    verticalHeader()->hide();
    setHorizontalHeader(new MemoryWordTableHeader(this));
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    setShowGrid(false);
    setItemDelegate(new MemoryWordDelegate(this));
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

}


MemoryWordView::~MemoryWordView() {}

} // namespace Gui
