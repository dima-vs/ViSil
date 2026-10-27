#pragma once

#include <QMainWindow>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QString>
#include <cstdint>

#include "debugger_panel.h"
#include "memory_instruction_view.h"
#include "memory_word_view.h"
#include "register_file_view.h"

namespace Gui
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    DebuggerPanel *m_debuggerPanel;

    MemoryInstructionView *m_instructionView = nullptr;
    RegisterFileView *m_registerView = nullptr;
    MemoryWordView *m_memoryWordView = nullptr;

    void setupUi();
    void linkSignals();
    void setupMenuBar();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void scrollToInstructionRow(int row);
    void setInstructionModel(QAbstractItemModel *model);
    void setRegisterModel(QAbstractItemModel *model);
    void setMemoryWordModel(QAbstractItemModel *model);
signals:
    // menu
    void fileSelected(const QString& fileName);
    // view
    void memBlockColumnsChanged(int newColumnCount);
    void memWordFormatCycled();
    void breakpointToggled(uint64_t addr, const QModelIndex &index);
    void regFormatCycled(const QModelIndex &index);
    void dbgRunClicked();
    void dbgPauseResumeClicked();
    void dbgStepIntoClicked();

private slots:
    void onFileOpen();
};

} // namespace Gui
