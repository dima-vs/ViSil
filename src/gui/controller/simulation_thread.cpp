#include "simulation_thread.h"

namespace Gui
{

SimulationThread::SimulationThread(
    Core::SimulationRunner& simRunner,
    QObject* parent
    ) : QThread(parent), m_simRunner(simRunner)
{
    m_simRunner.setOnStoppedCallback([this](Core::StopReason reason, Core::ExecutionStatus exStatus){
        emit this->resultReady(exStatus, reason);
    });
}

void SimulationThread::run()
{
    switch (m_currentMode)
    {
    case RunningMode::RunLoop:
        m_simRunner.runLoop();
        break;
    case RunningMode::StepOnce:
        m_simRunner.stepOnce();
        break;
    }
}

void SimulationThread::setMode(RunningMode mode)
{
    m_currentMode = mode;
}

SimulationThread::~SimulationThread()
{
    m_simRunner.setOnStoppedCallback(nullptr);
}

} // namespace Gui
