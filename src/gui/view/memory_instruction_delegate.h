#pragma once

#include <QStyledItemDelegate>
#include <QPainter>
#include <QIcon>
#include <QPainterPath>
#include <QColor>
#include <cstdint>

#include "disassembler_interface.h"

namespace Gui
{

class MemoryInstructionDelegate : public QStyledItemDelegate
{
private:
    QIcon m_arrowIcon{":/icons/instr-view/right-arrow-96.png"};

    void drawCellText(
        QPainter *painter,
        const QRectF &rect,
        const QModelIndex &index
        ) const;

    void drawInstructionText(
        QPainter *painter,
        const QRectF &rect,
        const QModelIndex &index
        ) const;

    void drawBreakpointOrArrow(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index,
        bool isCurrentPc
        ) const;

    QColor resolveBackgroundColor(
        const QStyleOptionViewItem &option,
        bool isSideColumn,
        bool isCurrentPc
        ) const;

    QColor resolveColor(const Core::TokenType tokenType) const;
public:
    MemoryInstructionDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent) {}

    void paint(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index
        ) const override;
};

} // namespace Gui
