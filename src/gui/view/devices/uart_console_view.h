#pragma once

#include <QWidget>
#include <QPlainTextEdit>
#include <cstdint>

namespace Gui
{

class UartConsoleView : public QWidget
{
    Q_OBJECT

public:
    explicit UartConsoleView(QWidget* parent = nullptr);

public slots:
    void appendByte(uint8_t byte);
    void clearConsole();

private:
    QPlainTextEdit* m_textEdit = nullptr;
};

} // namespace Gui
