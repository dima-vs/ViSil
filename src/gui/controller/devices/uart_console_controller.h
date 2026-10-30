#pragma once

#include <QObject>
#include "gui/view/mainwindow.h"
#include "gui/app_context/app_models.h"

namespace Gui
{

class UartConsoleController : public QObject
{
    Q_OBJECT

public:
    UartConsoleController(
        AppModels& appModels,
        MainWindow& mainWindow,
        QObject* parent = nullptr
        );
};

} // namespace Gui
