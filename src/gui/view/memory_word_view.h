#pragma once

#include <QTableView>

namespace Gui
{

class MemoryWordView : public QTableView
{
    Q_OBJECT

public:
    MemoryWordView(QWidget *parent = nullptr);
    ~MemoryWordView();
};

} // namespace Gui
