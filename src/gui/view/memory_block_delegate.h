#pragma once

#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>

namespace Gui
{

class MemoryBlockDelegate : public QStyledItemDelegate
{
public:
    MemoryBlockDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent) {}

    void paint(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index
        ) const override;
};

} // namespace Gui
