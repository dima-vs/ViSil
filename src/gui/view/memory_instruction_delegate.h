#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>
#include <QColor>

#include "disassembler_interface.h"

namespace Gui
{

class MemoryInstructionDelegate : public QStyledItemDelegate
{
private:
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
