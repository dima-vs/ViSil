#include "uart_console_controller.h"
#include "gui/view/devices/uart_console_view.h"

namespace Gui
{

UartConsoleController::UartConsoleController(
    AppModels& appModels,
    MainWindow& mainWindow,
    QObject* parent
    )
    : QObject(parent)
{
    connect(&appModels.uartModel, &UartModel::byteReceived,
            &mainWindow, &MainWindow::uartAppendByte);

    connect(&mainWindow, &MainWindow::fileSelected,
            &mainWindow, &MainWindow::clearUartConsole);
}

} // namespace Gui
