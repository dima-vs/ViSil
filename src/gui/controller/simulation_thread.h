#pragma once

#include "common.h"
#include "simulation_runner.h"

#include <QThread>
#include <QObject>

namespace Gui
{

class SimulationThread : public QThread
{
    Q_OBJECT

public:
    enum class RunningMode
    {
        RunLoop,
        StepOnce
    };
private:
    Core::SimulationRunner& m_simRunner;
    RunningMode m_currentMode = RunningMode::RunLoop;
public:
    SimulationThread(
        Core::SimulationRunner& simRunner,
        QObject* parent = nullptr
        );

    void setMode(RunningMode mode);

    ~SimulationThread();
protected:
    void run() override;
signals:
    void resultReady(Core::ExecutionStatus status, Core::StopReason reason);
};

} // namespace Gui
