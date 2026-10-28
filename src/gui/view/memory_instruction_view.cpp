#include "memory_instruction_view.h"
#include "memory_instruction_types.h"
#include <QHeaderView>

#include <QScrollBar>
#include <cmath>

namespace Gui
{

MemoryInstructionView::MemoryInstructionView(QWidget *parent) :
    QTableView(parent)
{
    setMouseTracking(true);
    m_delegate = new MemoryInstructionDelegate(this);
    setItemDelegate(m_delegate);

    verticalHeader()->hide();
    horizontalHeader()->hide();

    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(24);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    setShowGrid(false);

    connect(this, &QTableView::clicked,
            this, &MemoryInstructionView::handleClick);

    m_errorCard = new InlineErrorCard(this->viewport());

    connect(verticalScrollBar(), &QScrollBar::valueChanged,
            this, &MemoryInstructionView::updateOverlayPositions);

    connect(horizontalScrollBar(), &QScrollBar::valueChanged,
            this, &MemoryInstructionView::updateOverlayPositions);
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

void MemoryInstructionView::scrollToRow(int row)
{
    if (model() && row >= 0 && row < model()->rowCount())
    {
        QModelIndex idx = model()->index(row, 0);
        scrollTo(idx, QAbstractItemView::PositionAtCenter);
    }
}

void MemoryInstructionView::resizeEvent(QResizeEvent *event)
{
    QTableView::resizeEvent(event);
    updateOverlayPositions();
}

void MemoryInstructionView::showErrorAtRow(int row, const QString& message)
{
    if (row < 0) return;

    m_errorRow = row;
    m_errorCard->setError(message);

    updateOverlayPositions();
}

void MemoryInstructionView::hideError()
{
    m_errorRow = -1;
    m_errorCard->hide();
}

void MemoryInstructionView::updateOverlayPositions()
{
    if (m_errorRow < 0 || !model())
    {
        m_errorCard->hide();
        return;
    }

    QModelIndex idx = model()->index(m_errorRow, static_cast<int>(MemInstrColumn::Disassembly));
    QRectF cellRect = visualRect(idx);

    if (!cellRect.isValid() || cellRect.isEmpty() ||
        cellRect.bottom() < 0 || cellRect.top() > viewport()->height())
    {
        m_errorCard->hide();
        return;
    }

    int tokenWidth = m_delegate->getInstrTokenWidth(idx);
    double cardX = cellRect.left() + tokenWidth;

    QRectF cardRect = m_errorCard->geometry();
    cardRect.moveCenter(QPointF(cardX + m_errorCard->width() / 2.0, cellRect.center().y()));

    cardRect.adjust(10, 0, 0, 0);
    m_errorCard->move(std::round(cardRect.left()), std::round(cardRect.top()));

    m_errorCard->raise();
    m_errorCard->show();
}

MemoryInstructionView::~MemoryInstructionView() {}


} // namespace Gui
