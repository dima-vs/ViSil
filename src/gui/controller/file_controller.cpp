#include "file_controller.h"
#include <QFile>
#include <QIODevice>
#include <QByteArray>
#include <cstdint>

namespace Gui
{

FileController::FileController(
    AppModels appModels,
    MainWindow& mainWindow,
    Core::IBusDevice& busDevice,
    QObject* parent
    ) : QObject(parent),
    m_appModels(appModels), m_mainWindow(mainWindow),
    m_busDevice(busDevice)
{
    connect(&m_mainWindow, &MainWindow::fileSelected,
        this, &FileController::onFileSelected);
}

void FileController::onFileSelected(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray rawBytes = file.readAll();
    if (rawBytes.isEmpty())
        return;

    const uint64_t startAddr = 0;

    if (static_cast<uint64_t>(rawBytes.size()) > m_busDevice.getSize())
    {
        return;
    }

    m_busDevice.writeBlock(
        startAddr,
        reinterpret_cast<const uint8_t*>(rawBytes.constData()),
        static_cast<size_t>(rawBytes.size())
        );

    m_appModels.refreshMemory();
    emit fileLoaded();
}

} // namespace Gui
