#pragma once

#include <QTableView>

class MemoryBlockView : public QTableView
{
    Q_OBJECT

private:
    int m_minCellWidth = 95;
    int m_maxColumns = 8;
    int m_currentColumns = 0;
public:
    MemoryBlockView(QWidget *parent = nullptr);
    ~MemoryBlockView();
signals:
    void columnsChanged(int newColumnCount);
protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;
};
