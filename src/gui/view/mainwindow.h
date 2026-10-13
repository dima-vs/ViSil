#pragma once

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
