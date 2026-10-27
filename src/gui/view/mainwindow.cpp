#include "mainwindow.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QFileInfo>

namespace Gui
{

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    linkSignals();
    setupMenuBar();

    resize(1200, 800);
    setWindowTitle("ViSiL Emulator & Debugger");
}

void MainWindow::setupUi()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    m_debuggerPanel = new DebuggerPanel(this);
    mainLayout->addWidget(m_debuggerPanel);

    QSplitter *vSplitter = new QSplitter(Qt::Vertical, centralWidget);
    QSplitter *hSplitter = new QSplitter(Qt::Horizontal, vSplitter);

    m_instructionView = new MemoryInstructionView(hSplitter);
    m_registerView = new RegisterFileView(hSplitter);

    hSplitter->addWidget(m_instructionView);
    hSplitter->addWidget(m_registerView);

    hSplitter->setStretchFactor(0, 3);
    hSplitter->setStretchFactor(1, 1);

    QTabWidget *memoryTabs = new QTabWidget(vSplitter);
    m_memoryWordView = new MemoryWordView(memoryTabs);

    memoryTabs->addTab(m_memoryWordView, "Memory Dump");

    vSplitter->addWidget(hSplitter);
    vSplitter->addWidget(memoryTabs);

    vSplitter->setStretchFactor(0, 2);
    vSplitter->setStretchFactor(1, 1);

    mainLayout->addWidget(vSplitter);
}

void MainWindow::linkSignals()
{
    connect(m_memoryWordView, &MemoryWordView::numericFormatCycleRequested,
            this, &MainWindow::memWordFormatCycled);
    connect(m_instructionView, &MemoryInstructionView::breakpointToggled,
            this, &MainWindow::breakpointToggled);
    connect(m_registerView, &RegisterFileView::numericFormatCycleRequested,
            this, &MainWindow::regFormatCycled);
    connect(m_debuggerPanel, &DebuggerPanel::runClicked,
            this, &MainWindow::dbgRunClicked);
    connect(m_debuggerPanel, &DebuggerPanel::pauseResumeClicked,
            this, &MainWindow::dbgPauseResumeClicked);
    connect(m_debuggerPanel, &DebuggerPanel::stepIntoClicked,
            this, &MainWindow::dbgStepIntoClicked);
}

void MainWindow::setupMenuBar()
{
    QMenuBar* mBar = menuBar();
    QMenu* fileMenu = new QMenu("File");
    QAction* fileOpen = new QAction("Open");

    connect(fileOpen, &QAction::triggered, this, &MainWindow::onFileOpen);

    fileMenu->addAction(fileOpen);
    mBar->addMenu(fileMenu);
}

void MainWindow::onFileOpen()
{
    static QString lastDirectory = "";

    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open Binary",
        lastDirectory,
        "Binary Files (*.bin);;All Files (*.*)"
        );

    if (fileName.isEmpty())
        return;

    lastDirectory = QFileInfo(fileName).absolutePath();
    emit fileSelected(fileName);
}

void MainWindow::scrollToInstructionRow(int row)
{
    m_instructionView->scrollToRow(row);
}

void MainWindow::setInstructionModel(QAbstractItemModel *model)
{
    m_instructionView->setModel(model);
}

void MainWindow::setRegisterModel(QAbstractItemModel *model)
{
    m_registerView->setModel(model);
}

void MainWindow::setMemoryWordModel(QAbstractItemModel *model)
{
    m_memoryWordView->setModel(model);
}

MainWindow::~MainWindow() {}

} // namespace Gui
