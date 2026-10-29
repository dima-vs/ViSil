#include "execution_flow_controller.h"
#include "simulation_thread.h"
#include "gui/utils/status_formatter.h"

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

    connect(this, &ExecutionFlowController::stateChanged,
            this, &ExecutionFlowController::hideErrorCard);

    connect(this, &ExecutionFlowController::simulationResultReady,
            this, &ExecutionFlowController::handleResults);

    m_simRunner.setOnStoppedCallback(
        [this](Core::StopReason reason, Core::ExecutionStatus exStatus) {
            emit this->simulationResultReady(exStatus, reason);
        }
    );
}

void ExecutionFlowController::onDbgRunClicked()
{
    if (isRunningState())
    {
        m_pendingAction = PendingAction::Restart;
        m_simRunner.requestStop();
        return;
    }

    m_cpu.reset();
    m_appModels.refreshCpu();
    m_simRunner.parkAtCurrentPc();

    changeState(State::RunningLoop);

    SimulationThread* simThread = new SimulationThread(m_simRunner);
    simThread->setMode(SimulationThread::RunningMode::RunLoop);
    connect(simThread, &SimulationThread::finished,
            simThread, &QObject::deleteLater);
    simThread->start();
}

void ExecutionFlowController::onDbgPauseResumeClicked()
{
    if (isRunningState())
    {
        m_simRunner.requestStop();
    }
    else if (m_currentState == State::Paused)
    {
        m_simRunner.parkAtCurrentPc();
        changeState(State::RunningLoop);

        SimulationThread* simThread = new SimulationThread(m_simRunner);
        simThread->setMode(SimulationThread::RunningMode::RunLoop);
        connect(simThread, &SimulationThread::finished, simThread, &QObject::deleteLater);
        simThread->start();
    }
}

void ExecutionFlowController::onDbgStepIntoClicked()
{
    m_simRunner.parkAtCurrentPc();

    changeState(State::RunningStep);

    SimulationThread* simThread = new SimulationThread(m_simRunner);
    simThread->setMode(SimulationThread::RunningMode::StepOnce);
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
        m_mainWindow.scrollToInstructionRow(targetRow);

    if (reason == Core::StopReason::CpuError)
    {
        QString errorMsg = StatusFormatter::formatStatus(status).second;
        m_mainWindow.showInstructionError(targetRow, errorMsg);
        changeState(State::Finished);
    }
    else if (reason == Core::StopReason::StepCompleted ||
        reason == Core::StopReason::UserRequested ||
        reason == Core::StopReason::BreakpointHit)
    {
        changeState(State::Paused);
    }
    else
    {
        changeState(State::Finished);
    }

    if (m_pendingAction == PendingAction::Restart)
    {
        m_pendingAction = PendingAction::None;
        onDbgRunClicked();
    }
}

void ExecutionFlowController::changeState(State newState)
{
    m_currentState = newState;

    switch (newState)
    {
    case State::Unloaded:
        m_mainWindow.setDebuggerRunEnabled(false);
        m_mainWindow.setDebuggerStepEnabled(false);
        m_mainWindow.setDebuggerPauseEnabled(false);
        break;

    case State::Ready:
        m_mainWindow.setDebuggerRunEnabled(true);
        m_mainWindow.setDebuggerStepEnabled(true);
        m_mainWindow.setDebuggerPauseEnabled(false);
        m_mainWindow.setDebuggerPauseMode(DebuggerPanel::PauseResumeMode::Pause);
        break;

    case State::RunningLoop:
        m_mainWindow.setDebuggerRunEnabled(true);
        m_mainWindow.setDebuggerStepEnabled(false);
        m_mainWindow.setDebuggerPauseEnabled(true);
        m_mainWindow.setDebuggerPauseMode(DebuggerPanel::PauseResumeMode::Pause);
        break;

    case State::RunningStep:
        m_mainWindow.setDebuggerRunEnabled(true);
        m_mainWindow.setDebuggerStepEnabled(false);
        m_mainWindow.setDebuggerPauseEnabled(false);
        m_mainWindow.setDebuggerPauseMode(DebuggerPanel::PauseResumeMode::Resume);
        break;

    case State::Paused:
        m_mainWindow.setDebuggerRunEnabled(true);
        m_mainWindow.setDebuggerStepEnabled(true);
        m_mainWindow.setDebuggerPauseEnabled(true);
        m_mainWindow.setDebuggerPauseMode(DebuggerPanel::PauseResumeMode::Resume);
        break;

    case State::Finished:
        m_mainWindow.setDebuggerRunEnabled(true);
        m_mainWindow.setDebuggerStepEnabled(false);
        m_mainWindow.setDebuggerPauseEnabled(false);
        break;
    }

    emit stateChanged(newState);
}

void ExecutionFlowController::onFileLoaded()
{
    changeState(State::Ready);
}

bool ExecutionFlowController::isRunningState()
{
    return (m_currentState == State::RunningLoop) ||
           (m_currentState == State::RunningStep);
}

void ExecutionFlowController::hideErrorCard(State newState)
{
    if (newState != State::Finished)
        m_mainWindow.hideInstructionError();
}

} // namespace Gui
