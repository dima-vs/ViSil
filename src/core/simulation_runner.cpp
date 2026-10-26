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
    if (m_breakpointsDirty.load(std::memory_order_relaxed))
    {
        std::lock_guard<std::mutex> lock(m_breakpointMutex);
        m_activeBreakpoints = m_sharedBreakpoints;
        m_breakpointsDirty.store(false, std::memory_order_relaxed);
    }

    uint64_t currentPc = m_cpu.getPc();
    auto it = m_activeBreakpoints.find(currentPc);
    if (it != m_activeBreakpoints.end())
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

void SimulationRunner::addBreakpoint(uint64_t addr)
{
    std::lock_guard<std::mutex> lock(m_breakpointMutex);
    m_sharedBreakpoints.insert(addr);
    m_breakpointsDirty.store(true, std::memory_order_relaxed);
}

void SimulationRunner::removeBreakpoint(uint64_t addr)
{
    std::lock_guard<std::mutex> lock(m_breakpointMutex);
    m_sharedBreakpoints.erase(addr);
    m_breakpointsDirty.store(true, std::memory_order_relaxed);
}

void SimulationRunner::clearBreakpoints()
{
    std::lock_guard<std::mutex> lock(m_breakpointMutex);
    m_sharedBreakpoints.clear();
    m_breakpointsDirty.store(true, std::memory_order_relaxed);
}

bool SimulationRunner::hasBreakpoint(uint64_t addr) const
{
    std::lock_guard<std::mutex> lock(m_breakpointMutex);
    return m_sharedBreakpoints.find(addr) != m_sharedBreakpoints.end();
}

void SimulationRunner::requestStop()
{
    m_stopRequested = true;
}

void SimulationRunner::setOnStoppedCallback(OnStoppedCallback cb)
{
    m_onStopped = cb;
}

} // namespace Core
