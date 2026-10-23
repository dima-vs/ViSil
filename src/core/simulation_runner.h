#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <unordered_set>
#include <mutex>
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
    mutable std::mutex m_breakpointMutex;
    OnStoppedCallback m_onStopped;

    StopReason executeStep(ExecutionStatus* outStatus = nullptr);
public:
    SimulationRunner(IExecutionEnvironment &execEnv,ICpu &cpu);

    void addBreakpoint(uint64_t addr);
    void removeBreakpoint(uint64_t addr);
    void clearBreakpoints();
    bool hasBreakpoint(uint64_t addr) const;

    void setOnStoppedCallback(OnStoppedCallback cb);
    void requestStop();

    StopReason stepOnce();
    void runSteps(uint64_t stepCount);
    void runLoop();
};

} // namespace Core
