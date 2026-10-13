#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    Q_INIT_RESOURCE(resources);

    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}
