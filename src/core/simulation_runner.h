#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <unordered_set>
#include "cpu_interface.h"
#include "execution_env_interface.h"

namespace Core
{

class SimulationRunner
{
private:
    using OnStoppedCallback = std::function<void(StopReason, ExecutionStatus)>;

    IExecutionEnvironment &m_execEnv;
    ICpu &m_cpu;
    std::unordered_set<uint64_t> m_breakpoints;
    std::atomic<bool> m_stopRequested{false};
    OnStoppedCallback m_onStopped;
    StopReason executeStep(ExecutionStatus* outStatus = nullptr);
public:
    SimulationRunner(IExecutionEnvironment &execEnv,ICpu &cpu);

    void addBreakpoint(uint64_t addr) { m_breakpoints.insert(addr); }
    void removeBreakpoint(uint64_t addr) { m_breakpoints.erase(addr); }
    void clearBreakpoints() { m_breakpoints.clear(); }
    void setOnStoppedCallback(OnStoppedCallback cb) { m_onStopped = cb; }
    void requestStop() { m_stopRequested = true; }

    StopReason stepOnce();
    void runSteps(uint64_t stepCount);
    void runLoop();
};

} // namespace Core
