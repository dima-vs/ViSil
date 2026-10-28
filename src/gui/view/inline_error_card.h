#pragma once

#include <QWidget>
#include <QLabel>

namespace Gui
{

class InlineErrorCard : public QWidget
{
    Q_OBJECT

private:
    struct RenderInfo
    {
        int arrowPartWidth = 14;
        int arrowHeight = 8;
    } m_renderInfo;

    QLabel* m_iconLabel;
    QLabel* m_messageLabel;

    void initLayout();
public:
    explicit InlineErrorCard(QWidget* parent = nullptr);
    void setError(const QString& message, const QString& errorCode = "");
protected:
    void paintEvent(QPaintEvent* event) override;
};

}
