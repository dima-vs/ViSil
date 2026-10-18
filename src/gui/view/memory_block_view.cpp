#include "memory_block_view.h"
#include "memory_block_delegate.h"
#include "theme_manager.h"
#include <QHeaderView>
#include <QObject>

namespace Gui
{

MemoryBlockView::MemoryBlockView(QWidget *parent) :
    QTableView(parent)
{
    horizontalHeader()->hide();
    verticalHeader()->hide();

    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(34);

    setShowGrid(false);
    setItemDelegate(new MemoryBlockDelegate(this));
}

void MemoryBlockView::resizeEvent(QResizeEvent *event)
{
    QTableView::resizeEvent(event);

    int viewWidth = this->viewport()->width();

    int calculatedColumns = viewWidth / m_minCellWidth;
    if (calculatedColumns < 1)
        calculatedColumns = 1;

    if (calculatedColumns > m_maxColumns)
    {
        calculatedColumns = m_maxColumns;
    }

    if (m_currentColumns != calculatedColumns)
    {
        m_currentColumns = calculatedColumns;
        emit columnsChanged(calculatedColumns);
    }
}

void MemoryBlockView::selectionChanged(
    const QItemSelection &selected,
    const QItemSelection &deselected
    )
{
    QTableView::selectionChanged(selected, deselected);
    viewport()->update();
}

void MemoryBlockView::paintEvent(QPaintEvent *event)
{
    QTableView::paintEvent(event);

    if (!selectionModel() || !selectionModel()->hasSelection())
        return;

    using TM = ThemeManager;
    const TM& theme = TM::instance();
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    QPen borderPen(
        theme.getColor(TM::GuiElementColor::MemBlock_SelectedRangeBorder),
        2
    );
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);

    for (const QItemSelectionRange &range : selectionModel()->selection())
    {
        if (!range.isValid())
            continue;

        QRect topLeftRect = visualRect(range.topLeft());
        QRect bottomRightRect = visualRect(range.bottomRight());

        QRectF selectionBoundingRect = topLeftRect.united(bottomRightRect);

        selectionBoundingRect.adjust(0.5, 0.5, -0.5, -0.5);
        painter.drawRoundedRect(selectionBoundingRect, 6, 6);
    }
}


MemoryBlockView::~MemoryBlockView() {}

} // namespace Gui
