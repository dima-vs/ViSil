#include "inline_error_card.h"
#include "theme_manager.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <cmath>

namespace Gui
{

InlineErrorCard::InlineErrorCard(QWidget* parent) : QWidget(parent)
{
    setObjectName("InlineErrorCard");

    setAttribute(Qt::WA_TransparentForMouseEvents);
    initLayout();
    hide();
}

void InlineErrorCard::initLayout()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(m_renderInfo.arrowPartWidth + 10, 6, 10, 6);
    layout->setSpacing(6);

    m_iconLabel = new QLabel("⚠️", this);
    m_iconLabel->setObjectName("ErrorIconLabel");

    m_messageLabel = new QLabel(this);
    m_messageLabel->setObjectName("ErrorMessageLabel");
    m_messageLabel->setTextFormat(Qt::TextFormat::RichText);
    m_messageLabel->setWordWrap(true);

    layout->addWidget(m_iconLabel);
    layout->addWidget(m_messageLabel);

    using TM = ThemeManager;
    const TM& theme = TM::instance();

    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(8);
    shadow->setOffset(0, 3);
    shadow->setColor(theme.getColor(TM::GuiElementColor::ErrorCard_DropShadowEffect));

    this->setGraphicsEffect(shadow);
}

void InlineErrorCard::setError(const QString& message, const QString& errorCode)
{
    if (errorCode.isEmpty())
        m_messageLabel->setText(message);
    else
        m_messageLabel->setText(QString("<b>%1</b>: %2").arg(errorCode, message));

    adjustSize();
}

void InlineErrorCard::paintEvent(QPaintEvent* event)
{
    using TM = ThemeManager;
    const TM& theme = TM::instance();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(theme.getColor(TM::GuiElementColor::ErrorCard_Background));
    painter.setPen(QPen(theme.getColor(TM::GuiElementColor::ErrorCard_Border), 1));

    int arrowHeight = m_renderInfo.arrowHeight;
    int arrowPartWidth = m_renderInfo.arrowPartWidth;

    QRect rect = this->rect();
    int centerY = rect.height() / 2;
    int radius = 4;

    // coordinates of the rect part given frame offset
    int left = rect.left() + arrowPartWidth + 1;
    int right = rect.right() - 1;
    int top = rect.top() + 1;
    int bottom = rect.bottom() - 1;

    QPainterPath path;

    // 1. Start from the arrow tip
    path.moveTo(1, centerY);

    // 2. Line to the top joint point
    path.lineTo(left, centerY - arrowHeight / 2);

    // 3. Top-left corner rounding
    path.lineTo(left, top + radius);
    path.arcTo(left, top, radius * 2, radius * 2, 180, -90);

    // 4. Top edge and top-right corner
    path.lineTo(right - radius, top);
    path.arcTo(right - radius * 2, top, radius * 2, radius * 2, 90, -90);

    // 5. Right edge and bottom-right corner
    path.lineTo(right, bottom - radius);
    path.arcTo(right - radius * 2, bottom - radius * 2, radius * 2, radius * 2, 0, -90);

    // 6. Bottom edge and bottom-left corner
    path.lineTo(left + radius, bottom);
    path.arcTo(left, bottom - radius * 2, radius * 2, radius * 2, 270, -90);

    // 7. Line to the bottom intersection with the arrow
    path.lineTo(left, centerY + arrowHeight / 2);

    // 8. Close the path back to the arrow tip
    path.lineTo(1, centerY);

    painter.drawPath(path);
}

}
