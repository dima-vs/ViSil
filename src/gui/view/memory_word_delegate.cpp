#include "memory_word_delegate.h"
#include "theme_manager.h"
#include <QLinearGradient>
#include <QPen>
#include <QBrush>

void MemoryWordDelegate::paint(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index
    ) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    drawBackground(painter, option, index);

    CellMetadata meta = index.data(Qt::UserRole).value<CellMetadata>();
    drawCellText(painter, option, index, meta);

    painter->restore();
}

void MemoryWordDelegate::drawBackground(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index
    ) const
{
    using TM = ThemeManager;
    const TM &theme = TM::instance();

    bool isSelected = option.state & QStyle::State_Selected;
    QColor bgColor;

    if (isSelected)
    {
        bgColor = theme.getColor(
            TM::GuiElementColor::MemWord_SelectedRowBackground
            );
    }
    else
    {
        bgColor = (index.row() % 2 == 0) ?
            theme.getColor(TM::GuiElementColor::MemWord_EvenRowBackground) :
            theme.getColor(TM::GuiElementColor::MemWord_OddRowBackground);
    }

    // expand rect to prevent rendering gaps between adjacent cells
    painter->fillRect(option.rect.adjusted(-1, 0, 1, 0), bgColor);
}

QFont MemoryWordDelegate::resolveFont(Column col) const
{
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

    if (col == Column::RawBytes || col == Column::NumericValue)
    {
        font.setBold(true);
    }
    else if (col == Column::Address)
    {
        font.setWeight(QFont::Medium);
    }

    return font;
}

MemoryWordDelegate::TextRenderInfo MemoryWordDelegate::computeTextRenderInfo(
    const QFontMetrics &fm,
    const QRectF &rect,
    const QString &rawText,
    const QString &prefix,
    int margin
    ) const
{
    TextRenderInfo txtRenderInf;
    int availableWidth = static_cast<int>(rect.width()) - (margin * 2);

    txtRenderInf.fullText = fm.elidedText(
        prefix + rawText, Qt::ElideRight, availableWidth
        );

    txtRenderInf.prefixPart = "";
    txtRenderInf.textPart = txtRenderInf.fullText;

    if (!prefix.isEmpty() && txtRenderInf.fullText.startsWith(prefix))
    {
        txtRenderInf.prefixPart = prefix;
        txtRenderInf.textPart = txtRenderInf.fullText.sliced(prefix.length());
        txtRenderInf.prefixWidth = fm.horizontalAdvance(txtRenderInf.prefixPart);
    }

    txtRenderInf.totalWidth = fm.horizontalAdvance(txtRenderInf.fullText);



    // calculate baseline
    double xPos = rect.left() + (rect.width() - txtRenderInf.totalWidth) / 2.0;
    double yPos = rect.top() +
                  (rect.height() - fm.ascent() - fm.descent()) / 2.0 +
                  fm.ascent();

    txtRenderInf.textPos = QPointF(xPos, yPos);
    return txtRenderInf;
}

void MemoryWordDelegate::drawCellText(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index,
    const CellMetadata &meta
    ) const
{
    using TM = ThemeManager;
    const TM &theme = TM::instance();

    QFont font = resolveFont(meta.column);
    painter->setFont(font);

    const int margin = 5;
    QString rawText = index.data(Qt::DisplayRole).toString();
    TextRenderInfo txtRenderInf = computeTextRenderInfo(
        QFontMetrics(font), option.rect, rawText, meta.prefix, margin);

    switch (meta.column)
    {
    case Column::RawBytes:
        drawGradientText(
            painter,
            txtRenderInf,
            meta,
            theme.getColor(TM::GuiElementColor::MemWord_RawBytesCellText)
            );
        break;

    case Column::Address:
        drawTextWithPrefix(
            painter,
            txtRenderInf,
            theme.getColor(TM::GuiElementColor::MemWord_AddressCellText)
            );
        break;

    case Column::NumericValue:
        drawTextWithPrefix(
            painter,
            txtRenderInf,
            theme.getColor(TM::GuiElementColor::MemWord_NumericValCellText)
            );
        break;

    default:
        drawPlainText(
            painter,
            txtRenderInf,
            theme.getColor(TM::GuiElementColor::MemWord_DefaultCellText)
            );
        break;
    }
}

void MemoryWordDelegate::drawGradientText(
    QPainter *painter,
    const TextRenderInfo &txtRenderInf,
    const CellMetadata &meta,
    const QColor &brightColor
    ) const
{
    QLinearGradient linearGrad(
        txtRenderInf.textPos,
        QPointF(
            txtRenderInf.textPos.x() + txtRenderInf.totalWidth,
            txtRenderInf.textPos.y())
        );

    const int darkFactor = 170;

    QColor firstColor = (meta.endianness == Core::Endianness::Big) ?
                            brightColor : brightColor.darker(darkFactor);
    QColor lastColor = (meta.endianness == Core::Endianness::Big) ?
                           brightColor.darker(darkFactor) : brightColor;

    linearGrad.setColorAt(0.0, firstColor);
    linearGrad.setColorAt(1.0, lastColor);

    painter->setPen(QPen(QBrush(linearGrad), 1.0));
    painter->drawText(txtRenderInf.textPos, txtRenderInf.fullText);
}

void MemoryWordDelegate::drawTextWithPrefix(
    QPainter *painter,
    const TextRenderInfo &txtRenderInf,
    const QColor &textColor
    ) const
{
    if (!txtRenderInf.prefixPart.isEmpty())
    {
        // dim prefix color
        QColor dimColor = textColor;
        dimColor.setAlpha(100);

        painter->setPen(dimColor);
        painter->drawText(txtRenderInf.textPos, txtRenderInf.prefixPart);

        painter->setPen(textColor);
        QPointF bodyPos(
            txtRenderInf.textPos.x() + txtRenderInf.prefixWidth,
            txtRenderInf.textPos.y()
            );
        painter->drawText(bodyPos, txtRenderInf.textPart);
    }
    else
    {
        drawPlainText(painter, txtRenderInf, textColor);
    }
}

void MemoryWordDelegate::drawPlainText(
    QPainter *painter,
    const TextRenderInfo &txtRenderInf,
    const QColor &textColor
    ) const
{
    painter->setPen(textColor);
    painter->drawText(txtRenderInf.textPos, txtRenderInf.fullText);
}
