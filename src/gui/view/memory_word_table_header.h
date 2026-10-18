#pragma once
#include <QHeaderView>
#include <QMap>
#include <QVariantAnimation>
#include "theme_manager.h"

namespace Gui
{

class MemoryWordTableHeader : public QHeaderView
{
    Q_OBJECT

private:

    int m_hoveredSection = -1;
    int m_pressedSection = -1;

    const QColor m_baseColor;

    QMap<int, QColor> m_sectionColors;
    QMap<int, QVariantAnimation*> m_animations;

    void updateSectionColor(int logicalIndex, const QColor &targetColor);
    QColor getTargetColorForSection(int logicalIndex) const;
public:
    MemoryWordTableHeader(QWidget *parent = nullptr);
    ~MemoryWordTableHeader() override;

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};

} // namespace Gui
