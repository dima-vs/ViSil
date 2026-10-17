#include "memory_word_table_header.h"
#include <QPainter>
#include <QPen>
#include <QRectF>
#include <QPointF>
#include <QMouseEvent>
#include "theme_manager.h"

MemoryWordTableHeader::MemoryWordTableHeader(QWidget *parent) :
    QHeaderView(Qt::Orientation::Horizontal, parent),
    m_baseColor(ThemeManager::instance().getColor(
          ThemeManager::GuiElementColor::MemWord_HeaderSectionBackground))
{
    setSectionsClickable(true);
    setMouseTracking(true);
}

MemoryWordTableHeader::~MemoryWordTableHeader() {}

QColor MemoryWordTableHeader::getTargetColorForSection(int logicalIndex) const
{
    if (logicalIndex == m_pressedSection)
    {
        return m_baseColor.lighter(140);
    }
    if (logicalIndex == m_hoveredSection)
    {
        return m_baseColor.lighter(115);
    }
    return m_baseColor;
}

void MemoryWordTableHeader::updateSectionColor(int logicalIndex, const QColor &targetColor)
{
    if (logicalIndex < 0)
        return;

    if (!m_animations.contains(logicalIndex))
    {
        auto* anim = new QVariantAnimation(this);
        anim->setDuration(150);
        anim->setEasingCurve(QEasingCurve::OutCubic);

        connect(anim, &QVariantAnimation::valueChanged, this,
                [this, logicalIndex](const QVariant &value) {
                    m_sectionColors[logicalIndex] = value.value<QColor>();
                    updateSection(logicalIndex);
                }
        );

        m_animations[logicalIndex] = anim;
    }

    QVariantAnimation* anim = m_animations[logicalIndex];

    if (anim->endValue().value<QColor>() == targetColor)
    {
        return;
    }

    QColor startColor = m_sectionColors.value(logicalIndex, m_baseColor);

    anim->stop();
    anim->setStartValue(startColor);
    anim->setEndValue(targetColor);
    anim->start();
}

void MemoryWordTableHeader::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    if (!rect.isValid())
        return;

    using TM = ThemeManager;
    const TM &theme = TM::instance();

    painter->save();

    QRectF rectF{rect};
    QColor bgColor = m_sectionColors.value(logicalIndex, m_baseColor);
    painter->fillRect(rectF, bgColor);

    // draw text
    QVariant headerText = model()->headerData(
        logicalIndex,
        orientation(),
        Qt::DisplayRole
        );

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(10);
    painter->setPen(
        theme.getColor(TM::GuiElementColor::MemWord_HeaderSectionText)
        );
    painter->setFont(font);

    painter->drawText(
        rectF.adjusted(5, 0, -5, 0),
        Qt::AlignCenter,
        headerText.toString()
        );

    // bottom line
    const double lineWidth = 1.0;
    QPen pen;
    pen.setColor(
        theme.getColor(TM::GuiElementColor::MemWord_HeaderBottomLine)
        );
    pen.setWidthF(lineWidth);
    painter->setPen(pen);

    QPointF bottomLeft = rectF.bottomLeft() - QPointF(0, lineWidth);
    QPointF bottomRight = rectF.bottomRight() - QPointF(0, lineWidth);
    painter->drawLine(bottomLeft, bottomRight);

    painter->restore();
}

void MemoryWordTableHeader::mouseMoveEvent(QMouseEvent *event)
{
    int currentHovered = logicalIndexAt(event->pos());

    if (currentHovered != m_hoveredSection)
    {
        int oldHovered = m_hoveredSection;
        m_hoveredSection = currentHovered;

        if (oldHovered != -1 && oldHovered != m_pressedSection)
        {
            updateSectionColor(
                oldHovered,
                getTargetColorForSection(oldHovered)
                );
        }
        if (m_hoveredSection != -1 && m_hoveredSection != m_pressedSection)
        {
            updateSectionColor(
                m_hoveredSection,
                getTargetColorForSection(m_hoveredSection)
                );
        }
    }

    QHeaderView::mouseMoveEvent(event);
}

void MemoryWordTableHeader::leaveEvent(QEvent *event)
{
    int oldHovered = m_hoveredSection;
    int oldPressed = m_pressedSection;

    m_hoveredSection = -1;
    m_pressedSection = -1;

    updateSectionColor(oldHovered, m_baseColor);
    updateSectionColor(oldPressed, m_baseColor);

    QHeaderView::leaveEvent(event);
}

void MemoryWordTableHeader::mousePressEvent(QMouseEvent *event)
{
    QHeaderView::mousePressEvent(event);

    int currentPressed = logicalIndexAt(event->pos());
    if (currentPressed != -1)
    {
        m_pressedSection = currentPressed;
        updateSectionColor(
            m_pressedSection,
            getTargetColorForSection(m_pressedSection)
            );
    }
}

void MemoryWordTableHeader::mouseReleaseEvent(QMouseEvent *event)
{
    int oldPressed = m_pressedSection;
    m_pressedSection = -1;

    if (oldPressed != -1)
    {
        updateSectionColor(
            oldPressed,
            getTargetColorForSection(oldPressed)
            );
    }

    QHeaderView::mouseReleaseEvent(event);
}

void MemoryWordTableHeader::mouseDoubleClickEvent(QMouseEvent *event)
{
    MemoryWordTableHeader::mousePressEvent(event);
}
