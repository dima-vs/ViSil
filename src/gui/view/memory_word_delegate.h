#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QPainter>
#include <QPointF>
#include <QString>
#include <QModelIndex>
#include "memory_word_types.h"

class MemoryWordDelegate : public QStyledItemDelegate
{
private:
    struct TextRenderInfo
    {
        QString fullText;
        QString prefixPart;
        QString textPart;
        QPointF textPos;
        double totalWidth = 0.0;
        double prefixWidth = 0.0;
    };

    void drawBackground(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index
        ) const;

    void drawCellText(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index,
        const CellMetadata &meta
        ) const;

    QFont resolveFont(Column col) const;
    TextRenderInfo computeTextRenderInfo(
        const QFontMetrics &fm,
        const QRectF &rect,
        const QString &rawText,
        const QString &prefix,
        int margin
        ) const;

    void drawGradientText(
        QPainter *painter,
        const TextRenderInfo &txtRenderInf,
        const CellMetadata &meta,
        const QColor &brightColor
        ) const;

    void drawTextWithPrefix(
        QPainter *painter,
        const TextRenderInfo &txtRenderInf,
        const QColor &textColor
        ) const;

    void drawPlainText(
        QPainter *painter,
        const TextRenderInfo &txtRenderInf,
        const QColor &textColor
        ) const;
public:
    MemoryWordDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent) {}

    void paint(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index
        ) const override;
};
