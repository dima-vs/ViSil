#include "simulation_runner.h"

namespace Core
{

SimulationRunner::SimulationRunner(IExecutionEnvironment &execEnv,ICpu &cpu) :
    m_execEnv(execEnv), m_cpu(cpu)
{}

StopReason SimulationRunner::stepOnce()
{
    return executeStep();
}

void SimulationRunner::runSteps(uint64_t stepCount)
{
    m_stopRequested = false;
    StopReason reason = StopReason::StepCompleted;
    ExecutionStatus lastStatus = ExecutionStatus::Ok;

    for (uint64_t i = 0; i < stepCount && !m_stopRequested; ++i)
    {
        reason = executeStep(&lastStatus);
        if (reason != StopReason::StepCompleted)
            break;
    }

    if (m_onStopped)
        m_onStopped(reason, lastStatus);
}

void SimulationRunner::runLoop()
{
    m_stopRequested = false;
    StopReason reason = StopReason::StepCompleted;
    ExecutionStatus lastStatus = ExecutionStatus::Ok;

    while (!m_stopRequested)
    {
        reason = executeStep(&lastStatus);
        if (reason != StopReason::StepCompleted)
            break;
    }

    if (m_stopRequested)
        reason = StopReason::UserRequested;

    if (m_onStopped)
        m_onStopped(reason, lastStatus);
}

StopReason SimulationRunner::executeStep(ExecutionStatus* outStatus)
{
    uint64_t currentPc = m_cpu.getPc();
    auto it = m_breakpoints.find(currentPc);
    if (it != m_breakpoints.end())
        return StopReason::BreakpointHit;

    ExecutionStatus status = m_cpu.step();
    if (outStatus) *outStatus = status;

    if (isError(status))
        return StopReason::CpuError;

    if (status == ExecutionStatus::BreakpointHit)
        return StopReason::BreakpointHit;

    if (status == ExecutionStatus::Halted)
        return StopReason::ExecutionHalted;

    if (status == ExecutionStatus::EnvCall)
    {
        bool canContinue = m_execEnv.handleEnvCall(m_cpu);
        if (!canContinue)
            return StopReason::SyscallHalt;
    }

    return StopReason::StepCompleted;
}

} // namespace Core
