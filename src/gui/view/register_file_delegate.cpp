#include <QString>
#include <QRectF>
#include <QTextOption>
#include <QFontMetrics>

#include "register_file_delegate.h"
#include "register_file_types.h"
#include "theme_manager.h"

namespace Gui
{


void RegisterFileDelegate::paint(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index
    ) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    using TM = ThemeManager;
    const TM& theme = TM::instance();

    bool isSelected = option.state & QStyle::State_Selected;

    QRectF rect = option.rect;
    rect.adjust(m_margin, m_margin, -m_margin, -m_margin);

    rect.adjust(0.5, 0.5, -0.5, -0.5);
    int cornerRadius = 4;

    QPainterPath path;
    path.addRoundedRect(rect, cornerRadius, cornerRadius);

    QColor baseBgColor = theme.getColor(TM::GuiElementColor::RegFile_CellBackground);

    // for PC set another color
    if (index.row() == 0 && !isSelected)
    {
        baseBgColor = theme.getColor(TM::GuiElementColor::RegFile_PcBackground);
    }

    painter->fillPath(path, baseBgColor);

    QLinearGradient borderGradient(rect.topLeft(), rect.bottomLeft());

    if (isSelected)
    {
        borderGradient.setColorAt(0.0, theme.getColor(TM::GuiElementColor::RegFile_SelectedBorderStart));
        borderGradient.setColorAt(0.4, theme.getColor(TM::GuiElementColor::RegFile_SelectedBorderMid));
        borderGradient.setColorAt(1.0, theme.getColor(TM::GuiElementColor::RegFile_SelectedBorderEnd));
    }
    else if (index.row() == 0)
    {
        borderGradient.setColorAt(0.0, theme.getColor(TM::GuiElementColor::RegFile_PcBorderStart));
        borderGradient.setColorAt(0.2, theme.getColor(TM::GuiElementColor::RegFile_PcBorderMid));
        borderGradient.setColorAt(1.0, theme.getColor(TM::GuiElementColor::RegFile_PcBorderEnd));
    }
    else
    {
        borderGradient.setColorAt(0.0, theme.getColor(TM::GuiElementColor::RegFile_NormalBorderStart));
        borderGradient.setColorAt(0.15, theme.getColor(TM::GuiElementColor::RegFile_NormalBorderMid));
        borderGradient.setColorAt(1.0, theme.getColor(TM::GuiElementColor::RegFile_NormalBorderEnd));
    }

    QPen borderPen(QBrush(borderGradient), 1.0);
    painter->setPen(borderPen);
    painter->drawPath(path);

    drawCellText(painter, rect, index);

    painter->restore();
}

void RegisterFileDelegate::drawCellText(
    QPainter *painter,
    const QRectF &rect,
    const QModelIndex &index
    ) const
{
    using TM = ThemeManager;
    const TM& theme = TM::instance();

    QFont font;
    font.setFamilies({
        "Consolas",
        "Menlo",
        "Fira Code",
        "JetBrains Mono",
        "DejaVu Sans Mono"
    });

    font.setStyleHint(QFont::Monospace);
    font.setPointSize(11);
    font.setBold(true);
    painter->setFont(font);

    QString regName = index.data(static_cast<int>(RegisterFileRole::RegisterNameRole))
                          .toString();
    QString regVal = index.data(static_cast<int>(RegisterFileRole::RegisterValueRole))
                         .toString();
    QString prefix = index.data(static_cast<int>(RegisterFileRole::ValuePrefixRole))
                         .toString();

    // layout ratio: divideFactor for name, (1-divideFactor) for value
    double divideFactor = 0.4;
    int textMargin = 5;
    QRectF nameRect = QRectF(
        rect.x() + textMargin, rect.y(),
        rect.width() * divideFactor,
        rect.height()
        );

    QRectF valRect = QRectF(
        rect.x() + rect.width() * divideFactor,
        rect.y(),
        rect.width() * (1 - divideFactor) - textMargin,
        rect.height()
        );

    QString valWithPrefix = prefix + regVal;

    QFontMetrics fm(font);
    regName = fm.elidedText(regName, Qt::ElideRight, nameRect.width());
    valWithPrefix = fm.elidedText(valWithPrefix, Qt::ElideRight, valRect.width());

    if (!prefix.isEmpty() && valWithPrefix.startsWith(prefix))
    {
        valWithPrefix = valWithPrefix.mid(prefix.size());
    }
    else
    {
        prefix = "";
    }

    painter->setPen(theme.getColor(TM::GuiElementColor::RegFile_RegNameText));
    painter->drawText(
        nameRect,
        regName,
        QTextOption(Qt::AlignLeft | Qt::AlignVCenter)
        );

    int prefixWidth = prefix.isEmpty() ? 0 : fm.horizontalAdvance(prefix);
    int valueWidth = fm.horizontalAdvance(valWithPrefix);
    int totalWidth = prefixWidth + valueWidth;

    if (valRect.width() > totalWidth)
    {
        valRect.setLeft(valRect.right() - totalWidth);
    }

    if (prefixWidth > 0)
    {
        QRectF prefixRect = valRect;
        prefixRect.setWidth(prefixWidth);

        painter->setPen(theme.getColor(TM::GuiElementColor::RegFile_ValuePrefixText));
        painter->drawText(
            prefixRect,
            prefix,
            QTextOption(Qt::AlignLeft | Qt::AlignVCenter)
            );

        valRect.adjust(prefixWidth, 0, 0, 0);
    }

    painter->setPen(theme.getColor(TM::GuiElementColor::RegFile_ValueText));
    painter->drawText(
        valRect,
        valWithPrefix,
        QTextOption(Qt::AlignLeft | Qt::AlignVCenter)
        );

}

QSize RegisterFileDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    return QSize(size.width() + (m_margin * 2), size.height() + (m_margin * 2));
}

}
