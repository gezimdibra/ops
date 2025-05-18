#ifndef TYPES_H
#define TYPES_H

#include <string>

// Enum for process state
enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
};

// String representation of process states
inline std::string processStateToString(ProcessState state) {
    switch (state) {
        case ProcessState::NEW: return "new";
        case ProcessState::READY: return "ready";
        case ProcessState::RUNNING: return "running";
        case ProcessState::BLOCKED: return "blocked";
        case ProcessState::TERMINATED: return "terminated";
        default: return "unknown";
    }
}

// Enum for burst type
enum class BurstType {
    CPU,
    IO
};

// Enum for event type
enum class EventType {
    PROCESS_ARRIVAL,
    CPU_BURST_COMPLETION,
    IO_BURST_COMPLETION,
    TIME_SLICE_EXPIRATION,
    PROCESS_TERMINATED
};

// String representation of event types
inline std::string eventTypeToString(EventType type) {
    switch (type) {
        case EventType::PROCESS_ARRIVAL: return "Process Arrival";
        case EventType::CPU_BURST_COMPLETION: return "CPU Burst Completion";
        case EventType::IO_BURST_COMPLETION: return "IO Burst Completion";
        case EventType::TIME_SLICE_EXPIRATION: return "Time Slice Expiration";
        case EventType::PROCESS_TERMINATED: return "Process Terminated";
        default: return "Unknown Event";
    }
}

// String representation of scheduling algorithms
inline std::string algorithmToString(const std::string& alg) {
    if (alg == "FCFS") return "First Come First Serve";
    if (alg == "SJF") return "Shortest Job First";
    if (alg == "SRTN") return "Shortest Remaining Time Next";
    if (alg.substr(0, 2) == "RR") {
        std::string quantum = alg.substr(2);
        return "Round Robin (Quantum " + quantum + ")";
    }
    return "Unknown Algorithm";
}

#endif // TYPES_H