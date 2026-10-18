#include "memory_block_delegate.h"
#include "theme_manager.h"

namespace Gui
{

void MemoryBlockDelegate::paint(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index
    ) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    using TM = ThemeManager;
    const TM& theme = TM::instance();

    bool isSelected = false;

    isSelected = option.state & QStyle::State_Selected;

    QColor bgColor;
    if (isSelected)
    {
        bgColor = theme.getColor(TM::GuiElementColor::MemBlock_SelectedCellBackground);
    }
    else
    {
        if (index.row() % 2 == 0)
            bgColor = theme.getColor(TM::GuiElementColor::MemBlock_EvenCellBackground);
        else
            bgColor = theme.getColor(TM::GuiElementColor::MemBlock_OddCellBackground);
    }

    QRectF rect = option.rect;
    rect.adjust(0.5, 0.5, -0.5, -0.5);
    int cornerRadius = 4;

    QPainterPath path;
    path.addRoundedRect(rect, cornerRadius, cornerRadius);

    painter->fillPath(path, bgColor);

    QColor borderColor = isSelected ?
        theme.getColor(TM::GuiElementColor::MemBlock_SelectedCellBorder) :
        theme.getColor(TM::GuiElementColor::MemBlock_CellBorder);

    painter->setPen(QPen(borderColor, 1));
    painter->drawPath(path);

    QStyleOptionViewItem textOption = option;

    QFont font;
    font.setBold(true);
    textOption.font = font;

    textOption.displayAlignment = Qt::AlignCenter;
    textOption.palette.setColor(
        QPalette::Text,
        theme.getColor(TM::GuiElementColor::MemBlock_CellText)
        );

    QStyledItemDelegate::paint(painter, textOption, index);

    painter->restore();
}

} // namespace Gui
