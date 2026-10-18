#include <QApplication>
#include <QHeaderView>
#include <QDebug>

#include "mainwindow.h"

#include "theme_manager.h"
#include "memory_block_model.h"
#include "memory_block_view.h"
#include "ram.h"
#include "common.h"

#include "memory_word_model.h"
#include "memory_word_view.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Q_INIT_RESOURCE(resources);

    // MainWindow mainWindow;
    // mainWindow.show();

    Gui::ThemeManager::instance().setTheme(Gui::ThemeManager::ThemeType::Dark);

    // Devices::Ram ram(1000, 256);
    // Gui::MemoryBlockModel model(ram);
    // Gui::MemoryBlockView view;

    // view.setModel(&model);
    // QObject::connect(&view, &Gui::MemoryBlockView::columnsChanged,
    //                  &model, &Gui::MemoryBlockModel::setGridColumns);

    // view.show();


    Devices::Ram ram(
        1000, 256,
        Devices::RamUninitBehavior::ZeroFill,
        Core::Endianness::Little
        );

    ram.write32(0, 0x12345678);
    ram.write32(4, 0xABCDEF11);
    ram.write32(8, 0xABCDEF11);
    ram.write32(16, 0xABCDEF22);
    ram.write32(20, 0xABCDEF33);
    ram.write32(24, 0xFBCDEF44);

    Gui::MemoryWordModel model(ram);
    model.setAddressWidth(Core::BitWidth::Bits32);
    model.setDisplayWordWidth(Core::BitWidth::Bits32);
    model.inspectBlock(0, 64);

    Gui::MemoryWordView view;

    view.setModel(&model);
    QObject::connect(
        view.horizontalHeader(), &QHeaderView::sectionClicked,
        &model, &Gui::MemoryWordModel::onHeaderSectionClicked
        );

    view.show();

    return a.exec();
}
