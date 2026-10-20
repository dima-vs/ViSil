#include "register_file_view.h"
#include <QHeaderView>
#include "register_file_delegate.h"

namespace Gui
{

RegisterFileView::RegisterFileView(QWidget *parent) :
    QTableView(parent)
{
    verticalHeader()->hide();
    horizontalHeader()->hide();

    setItemDelegate(new RegisterFileDelegate(this));
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(34);

    setShowGrid(false);
}


void RegisterFileView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);

    if (model)
    {
        // init spans
        for (int row = 0; row < model->rowCount(); ++row)
        {
            for (int col = 0; col < model->columnCount(); ++col)
            {
                QSize cellSpan = model->span(model->index(row, col));
                if (cellSpan.height() > 1 || cellSpan.width() > 1)
                    setSpan(row, col, cellSpan.height(), cellSpan.width());
            }
        }
    }
}

RegisterFileView::~RegisterFileView() {}


} // namespace Gui
