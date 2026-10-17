#pragma once

#include <QTableView>

class MemoryWordView : public QTableView
{
    Q_OBJECT

public:
    MemoryWordView(QWidget *parent = nullptr);
    ~MemoryWordView();
};
