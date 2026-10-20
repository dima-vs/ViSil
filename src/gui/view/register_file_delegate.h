#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>
#include <QSize>

namespace Gui
{

class RegisterFileDelegate : public QStyledItemDelegate
{
private:
    int m_margin;

    void drawCellText(
        QPainter *painter,
        const QRectF &rect,
        const QModelIndex &index
        ) const;
public:
    RegisterFileDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent),
        m_margin(3)
    {}

    QSize sizeHint(
        const QStyleOptionViewItem &option,
        const QModelIndex &index
        ) const override;

    void paint(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index
        ) const override;
};

} // namespace Gui
