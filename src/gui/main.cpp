#include <QApplication>
#include "mainwindow.h"

#include "theme_manager.h"
#include "memory_block_model.h"
#include "memory_block_view.h"
#include "ram.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Q_INIT_RESOURCE(resources);

    // MainWindow mainWindow;
    // mainWindow.show();

    ThemeManager::instance().setTheme(ThemeManager::ThemeType::Dark);

    Devices::Ram ram(1000, 256);
    MemoryBlockModel model(ram);
    MemoryBlockView view;

    view.setModel(&model);
    QObject::connect(&view, &MemoryBlockView::columnsChanged,
                     &model, &MemoryBlockModel::setGridColumns);

    view.show();

    return a.exec();
}
