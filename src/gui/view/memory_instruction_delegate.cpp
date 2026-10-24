#include "memory_instruction_delegate.h"
#include "memory_instruction_types.h"
#include "theme_manager.h"

#include <QRectF>
#include <QFont>
#include <QFontMetrics>
#include <QTextOption>
#include <algorithm>

namespace Gui
{

void MemoryInstructionDelegate::paint(
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
    QColor bgColor;

    int col = index.column();
    bool isSideColumn = (col == static_cast<int>(MemInstrColumn::Breakpoint) ||
                         col == static_cast<int>(MemInstrColumn::Address));

    if (isSideColumn)
    {
        bgColor = theme.getColor(TM::GuiElementColor::MemInstr_SidePanelCellBackground);
    }
    else
    {
        if (isSelected)
            bgColor = theme.getColor(TM::GuiElementColor::MemInstr_SelectedCellBackground);
        else
            bgColor = theme.getColor(TM::GuiElementColor::MemInstr_InstrCellBackground);
    }

    // expand rect to prevent rendering gaps between adjacent cells
    painter->fillRect(option.rect.adjusted(-1, 0, 1, 1), bgColor);

    // vertical separator between address and code space
    if (index.column() == static_cast<int>(MemInstrColumn::Address))
    {
        QColor separatorColor = theme.getColor(TM::GuiElementColor::MemInstr_AddrCodeSeparator);
        painter->setPen(QPen(separatorColor, 1));
        painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
    }

    drawCellText(painter, option.rect, index);

    // draw breakpoint
    if (index.column() == static_cast<int>(MemInstrColumn::Breakpoint))
    {
        bool isBreakPoint = index.data(static_cast<int>(MemInstrRole::BreakPointRole)).toBool();
        QRectF rect = option.rect;

        double squareSide = std::min(rect.width(), rect.height());
        QRectF square(0, 0, squareSide, squareSide);
        square.moveCenter(rect.center());
        int adjustment = 7;
        square.adjust(adjustment, adjustment, -adjustment, -adjustment);

        if (isBreakPoint)
        {
            QColor breakPointColor = theme.getColor(
                TM::GuiElementColor::MemInstr_BreakPoint
                );

            painter->setBrush(QBrush(breakPointColor));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(square);
        }
        else if (option.state & QStyle::State_MouseOver)
        {
            QColor breakPointHoverColor = theme.getColor(
                TM::GuiElementColor::MemInstr_BreakPointHover
                );

            painter->setBrush(QBrush(breakPointHoverColor));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(square);
        }
    }

    painter->restore();
}

void MemoryInstructionDelegate::drawCellText(
    QPainter *painter,
    const QRectF &rect,
    const QModelIndex &index
    ) const
{
    QString text = index.data(Qt::DisplayRole).toString();
    if (text.isEmpty())
        return;

    painter->save();

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

    using TM = ThemeManager;
    const TM& theme = TM::instance();

    int alignment = Qt::AlignLeft | Qt::AlignVCenter;
    QRectF textRect = rect;

    if (index.column() == static_cast<int>(MemInstrColumn::Disassembly))
    {
        textRect.adjust(5, 0, 0, 0);
        drawInstructionText(painter, textRect, index);
    } else
    {
        painter->setPen(
            theme.getColor(TM::GuiElementColor::MemInstr_AddressText)
            );
        painter->drawText(textRect, alignment, text);
    }


    painter->restore();
}

void MemoryInstructionDelegate::drawInstructionText(
    QPainter *painter,
    const QRectF &rect,
    const QModelIndex &index
    ) const
{
    Core::DisassembledInstr instr = index.data(
        static_cast<int>(MemInstrRole::DisassembledInstrRole))
        .value<Core::DisassembledInstr>();

    QFontMetrics fm(painter->font());

    double yPos = rect.top() + (rect.height() / 2.0) + ((fm.ascent() - fm.descent()) / 2.0);

    double currentX = rect.left();
    for (int i = 0; i < instr.tokens.size(); ++i)
    {
        Core::DisasmToken token = instr.tokens[i];
        QColor nextColor = resolveColor(token.type);
        QString tokenStr = QString::fromStdString(token.text);

        painter->setPen(nextColor);

        painter->drawText(QPointF(currentX, yPos), tokenStr);
        currentX += fm.horizontalAdvance(tokenStr);
    }
}

QColor MemoryInstructionDelegate::resolveColor(const Core::TokenType tokenType) const
{
    using TM = ThemeManager;
    const TM& theme = TM::instance();

    switch (tokenType)
    {
    case Core::TokenType::Mnemonic:
        return theme.getColor(TM::GuiElementColor::MemInstr_AsmTokenMnemonic);
    case Core::TokenType::Register:
        return theme.getColor(TM::GuiElementColor::MemInstr_AsmTokenRegister);
    case Core::TokenType::Address:
        return theme.getColor(TM::GuiElementColor::MemInstr_AsmTokenAddress);
    case Core::TokenType::Immediate:
        return theme.getColor(TM::GuiElementColor::MemInstr_AsmTokenImmediate);
    case Core::TokenType::Punctuation:
        return theme.getColor(TM::GuiElementColor::MemInstr_AsmTokenPunctuation);
    case Core::TokenType::Incorrect:
        return theme.getColor(TM::GuiElementColor::MemInstr_AsmTokenIncorrect);
    default:
        return theme.getColor(TM::GuiElementColor::MemInstr_AsmTokenDefault);
    }
}

} // namespace Gui
