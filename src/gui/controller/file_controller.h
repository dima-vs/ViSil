#pragma once

#include "bus_device_interface.h"
#include "mainwindow.h"
#include "app_models.h"

#include <QObject>
#include <QString>

namespace Gui
{

class FileController : public QObject
{
    Q_OBJECT

private:
    Core::IBusDevice& m_busDevice;
    AppModels m_appModels;
    MainWindow& m_mainWindow;
public:
    FileController(
        AppModels appModels,
        MainWindow& mainWindow,
        Core::IBusDevice& busDevice,
        QObject* parent = nullptr
        );
private slots:
    void onFileSelected(const QString& fileName);
};

} // namespace Gui
