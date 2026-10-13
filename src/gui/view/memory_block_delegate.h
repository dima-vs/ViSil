#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>

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
