#include "execution_flow_controller.h"
#include "simulation_thread.h"

namespace Gui
{

ExecutionFlowController::ExecutionFlowController(
    Core::ICpu &cpu,
    Core::SimulationRunner &simRunner,
    AppModels appModels,
    MainWindow& mainWindow,
    QObject* parent
    ) : QObject(parent), m_cpu(cpu), m_simRunner(simRunner),
    m_appModels(appModels), m_mainWindow(mainWindow)
{
    connect(&m_mainWindow, &MainWindow::dbgRunClicked,
            this, &ExecutionFlowController::onDbgRunClicked);

    connect(&m_mainWindow, &MainWindow::dbgPauseResumeClicked,
            this, &ExecutionFlowController::onDbgPauseResumeClicked);

    connect(&m_mainWindow, &MainWindow::dbgStepIntoClicked,
            this, &ExecutionFlowController::onDbgStepIntoClicked);

    connect(&m_mainWindow, &MainWindow::breakpointToggled,
            this, [this](uint64_t addr, const QModelIndex &index)
            {
                if (m_simRunner.hasBreakpoint(addr)) {
                    m_simRunner.removeBreakpoint(addr);
                } else {
                    m_simRunner.addBreakpoint(addr);
                }

                QModelIndex topLeft = index.model()->index(index.row(), 0);
                QModelIndex bottomRight = index.model()->index(index.row(), index.model()->columnCount() - 1);

                m_appModels.instructions.dataChanged(topLeft, bottomRight, {});
            });

    connect(&m_mainWindow, &MainWindow::memWordFormatCycled,
            &m_appModels.memoryWords, &MemoryWordModel::cycleNumericFormat);

    connect(&m_mainWindow, &MainWindow::regFormatCycled,
            &m_appModels.registers, &RegisterFileModel::cycleFormat);
}

void ExecutionFlowController::onDbgRunClicked()
{
    m_cpu.reset();
    m_appModels.refreshCpu();

    SimulationThread* simThread = new SimulationThread(m_simRunner, this);
    simThread->setMode(SimulationThread::RunningMode::RunLoop);
    connect(simThread, &SimulationThread::resultReady,
            this, &ExecutionFlowController::handleResults);
    connect(simThread, &SimulationThread::finished,
            simThread, &QObject::deleteLater);
    simThread->start();
}

void ExecutionFlowController::onDbgPauseResumeClicked()
{
    m_simRunner.requestStop();
}

void ExecutionFlowController::onDbgStepIntoClicked()
{
    SimulationThread* simThread = new SimulationThread(m_simRunner, this);
    simThread->setMode(SimulationThread::RunningMode::StepOnce);
    connect(simThread, &SimulationThread::resultReady,
            this, &ExecutionFlowController::handleResults);
    connect(simThread, &SimulationThread::finished,
            simThread, &QObject::deleteLater);
    simThread->start();
}

void ExecutionFlowController::handleResults(Core::ExecutionStatus status, Core::StopReason reason)
{
    m_appModels.refreshAll();

    uint64_t currentPc = m_cpu.getPc();

    m_appModels.instructions.setCurrentPc(currentPc);

    int targetRow = m_appModels.instructions.getRowByAddress(currentPc);

    if (targetRow >= 0)
    {
        m_mainWindow.scrollToInstructionRow(targetRow);
    }
}

} // namespace Gui
