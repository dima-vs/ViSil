#include "memory_word_view.h"
#include "memory_word_delegate.h"
#include "memory_word_table_header.h"
#include "memory_word_types.h"

#include <QHeaderView>

namespace Gui
{

MemoryWordView::MemoryWordView(QWidget *parent) :
    QTableView(parent)
{
    verticalHeader()->hide();
    setHorizontalHeader(new MemoryWordTableHeader(this));
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(horizontalHeader(), &QHeaderView::sectionClicked,
            this, &MemoryWordView::onHeaderSectionClicked);

    setShowGrid(false);
    setItemDelegate(new MemoryWordDelegate(this));
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
}

void MemoryWordView::onHeaderSectionClicked(int logicalIndex)
{
    if (logicalIndex < 0 || logicalIndex >= static_cast<int>(MemWordColumn::Count))
        return;

    if (static_cast<MemWordColumn>(logicalIndex) == MemWordColumn::NumericValue)
    {
        emit numericFormatCycleRequested();
    }
}

MemoryWordView::~MemoryWordView() {}

} // namespace Gui
