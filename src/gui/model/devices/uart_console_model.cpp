#include "uart_console_model.h"

namespace Gui
{

UartModel::UartModel(Devices::Uart& uart, QObject* parent)
    : QObject(parent), m_uart(uart)
{
    m_uart.setDataReceivedCallback([this](uint8_t byte) {
        emit byteReceived(byte);
    });
}

} // namespace Gui
