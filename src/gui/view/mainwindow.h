#pragma once

#include <QMainWindow>

namespace Gui
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};

} // namespace Gui
