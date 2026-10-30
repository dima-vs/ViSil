#pragma once

#include <QObject>
#include <cstdint>
#include "devices/uart.h"

namespace Gui
{

class UartModel : public QObject
{
    Q_OBJECT

private:
    Devices::Uart& m_uart;
public:
    explicit UartModel(Devices::Uart& uart, QObject* parent = nullptr);

signals:
    void byteReceived(uint8_t byte);
};

} // namespace Gui
