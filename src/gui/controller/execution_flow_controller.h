#pragma once

#include "cpu_interface.h"
#include "simulation_runner.h"
#include "common.h"
#include "app_models.h"
#include "mainwindow.h"

#include <QObject>

namespace Gui
{

class ExecutionFlowController : public QObject
{
    Q_OBJECT

public:
    enum class State
    {
        Unloaded,
        Ready,
        RunningLoop,
        RunningStep,
        Paused,
        Finished
    };

    enum class PendingAction
    {
        None,
        Restart
    };
private:
    Core::ICpu& m_cpu;
    Core::SimulationRunner& m_simRunner;
    AppModels m_appModels;
    MainWindow& m_mainWindow;

    State m_currentState = State::Unloaded;
    PendingAction m_pendingAction = PendingAction::None;

    bool isRunningState();
public:
    ExecutionFlowController(
        Core::ICpu& cpu,
        Core::SimulationRunner& simRunner,
        AppModels appModels,
        MainWindow& mainWindow,
        QObject* parent = nullptr
        );

    void changeState(State newState);
signals:
    void stateChanged(State newState);
    void simulationResultReady(Core::ExecutionStatus status, Core::StopReason reason);
private slots:
    void onDbgRunClicked();
    void onDbgPauseResumeClicked();
    void onDbgStepIntoClicked();
    void handleResults(Core::ExecutionStatus status, Core::StopReason reason);

    void hideErrorCard(State newState);
public slots:
    void onFileLoaded();
};

} // namespace Gui
