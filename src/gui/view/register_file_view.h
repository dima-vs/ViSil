#pragma once

#include <QTableView>

namespace Gui
{

class RegisterFileView : public QTableView
{
    Q_OBJECT

public:
    RegisterFileView(QWidget *parent = nullptr);
    ~RegisterFileView();

    void setModel(QAbstractItemModel *model) override;
};

} // namespace Gui
