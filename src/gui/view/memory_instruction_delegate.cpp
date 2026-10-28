#include "memory_instruction_delegate.h"
#include "memory_instruction_types.h"
#include "theme_manager.h"

#include <QFontMetrics>
#include <QTextOption>
#include <algorithm>

namespace Gui
{

MemoryInstructionDelegate::MemoryInstructionDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    initFont();
}

void MemoryInstructionDelegate::paint(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index
    ) const
{
    painter->save();

    using TM = ThemeManager;
    const TM& theme = TM::instance();

    int col = index.column();
    bool isSideColumn = (col == static_cast<int>(MemInstrColumn::Breakpoint) ||
                         col == static_cast<int>(MemInstrColumn::Address));

    bool isCurrentPc = index.data(static_cast<int>(MemInstrRole::CurrentPcRole)).toBool();
    QColor bgColor = resolveBackgroundColor(option, isSideColumn, isCurrentPc);

    QRectF fillRect = option.rect;
    if (isSideColumn)
    {
        fillRect = fillRect.adjusted(0, -1, 0, 1);
    }
    painter->fillRect(fillRect, bgColor);

    // vertical separator between address and code space
    if (col == static_cast<int>(MemInstrColumn::Address))
    {
        QColor separatorColor = theme.getColor(TM::GuiElementColor::MemInstr_AddrCodeSeparator);
        QRectF separatorRect(option.rect.right(), fillRect.top(), 1, fillRect.height());

        painter->fillRect(separatorRect, separatorColor);
    }

    painter->setRenderHint(QPainter::Antialiasing);

    drawCellText(painter, option.rect, index);

    // draw breakpoint
    if (col == static_cast<int>(MemInstrColumn::Breakpoint))
    {
        drawBreakpointOrArrow(painter, option, index, isCurrentPc);
    }

    painter->restore();
}

QColor MemoryInstructionDelegate::resolveBackgroundColor(
    const QStyleOptionViewItem &option,
    bool isSideColumn,
    bool isCurrentPc
    ) const
{
    const auto& theme = ThemeManager::instance();

    if (isCurrentPc && !isSideColumn)
    {
        return theme.getColor(ThemeManager::GuiElementColor::MemInstr_CurrentPcCellBackground);
    }

    if (isSideColumn)
    {
        return theme.getColor(ThemeManager::GuiElementColor::MemInstr_SidePanelCellBackground);
    }

    if (option.state & QStyle::State_Selected)
    {
        return theme.getColor(ThemeManager::GuiElementColor::MemInstr_SelectedCellBackground);
    }

    return theme.getColor(ThemeManager::GuiElementColor::MemInstr_InstrCellBackground);
}

void MemoryInstructionDelegate::drawBreakpointOrArrow(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index,
    bool isCurrentPc
    ) const
{
    QRectF rect = option.rect;
    double squareSide = std::min(rect.width(), rect.height());
    QRectF square(0, 0, squareSide, squareSide);
    square.moveCenter(rect.center());

    if (isCurrentPc)
    {
        constexpr int arrowAdjustment = 2;
        square.adjust(arrowAdjustment, arrowAdjustment, -arrowAdjustment, -arrowAdjustment);

        m_arrowIcon.paint(painter, square.toRect(), Qt::AlignCenter);
        return;
    }

    bool isBreakPoint = index.data(static_cast<int>(MemInstrRole::BreakPointRole)).toBool();
    bool isHovered = option.state & QStyle::State_MouseOver;

    if (isBreakPoint || isHovered)
    {
        constexpr int bpAdjustment = 7;
        square.adjust(bpAdjustment, bpAdjustment, -bpAdjustment, -bpAdjustment);

        auto elColor = isBreakPoint ? ThemeManager::GuiElementColor::MemInstr_BreakPoint
                                    : ThemeManager::GuiElementColor::MemInstr_BreakPointHover;

        QColor color = ThemeManager::instance().getColor(elColor);
        painter->setBrush(QBrush(color));
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(square);
    }
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
    painter->setFont(m_font);

    using TM = ThemeManager;
    const TM& theme = TM::instance();

    int alignment = Qt::AlignLeft | Qt::AlignVCenter;
    QRectF textRect = rect;

    if (index.column() == static_cast<int>(MemInstrColumn::Disassembly))
    {
        textRect.adjust(5, 0, 0, 0);
        drawInstructionText(painter, textRect, index);
    }
    else
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

    for (const auto& token : instr.tokens)
    {
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

int MemoryInstructionDelegate::getInstrTokenWidth(const QModelIndex &index) const
{
    Core::DisassembledInstr instr = index.data(
                     static_cast<int>(MemInstrRole::DisassembledInstrRole))
                .value<Core::DisassembledInstr>();

    QFontMetrics fm(m_font);
    QString tokenStr = QString::fromStdString(instr.toString());

    return fm.horizontalAdvance(tokenStr);
}

void MemoryInstructionDelegate::initFont()
{
    m_font.setFamilies({
        "Consolas",
        "Menlo",
        "Fira Code",
        "JetBrains Mono",
        "DejaVu Sans Mono"
    });
    m_font.setStyleHint(QFont::Monospace);
    m_font.setPointSize(11);
    m_font.setBold(true);
}

} // namespace Gui
