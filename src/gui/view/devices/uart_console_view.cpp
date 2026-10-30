#include "uart_console_view.h"
#include <QVBoxLayout>
#include <QTextCursor>

namespace Gui
{

UartConsoleView::UartConsoleView(QWidget* parent) : QWidget(parent)
{
    setObjectName("UartConsoleView");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_textEdit = new QPlainTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setMaximumBlockCount(10000);

    layout->addWidget(m_textEdit);
}

void UartConsoleView::appendByte(uint8_t byte)
{
    if (byte == '\r')
        return;

    QTextCursor cursor = m_textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_textEdit->setTextCursor(cursor);

    m_textEdit->insertPlainText(QString(QChar(byte)));
    m_textEdit->ensureCursorVisible();
}

void UartConsoleView::clearConsole()
{
    m_textEdit->clear();
}

} // namespace Gui
