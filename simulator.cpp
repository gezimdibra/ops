#include <iostream>
#include <algorithm>
#include <iomanip>
#include "simulator.h"
#include "statistics.h"

Simulator::Simulator(std::vector<Process>& processes, int processSwitchOverhead)
    : processes(processes), 
      processSwitchOverhead(processSwitchOverhead),
      runningProcess(nullptr),
      lastReadyQueueIndex(0),
      nextProcessTime(0),
      cpuBusyTime(0) {}

void Simulator::runFCFS(bool detailedMode, bool verboseMode) {
    resetSimulation();
    runSimulation(
        "FCFS", 
        [this](int currentTime) { return selectFirstComeFirstServe(currentTime); },
        false, // non-preemptive
        0,     // no time quantum
        detailedMode,
        verboseMode
    );
}

void Simulator::runSJF(bool detailedMode, bool verboseMode) {
    resetSimulation();
    runSimulation(
        "SJF", 
        [this](int currentTime) { return selectShortestJobFirst(currentTime); },
        false, // non-preemptive
        0,     // no time quantum
        detailedMode,
        verboseMode
    );
}

void Simulator::runSRTN(bool detailedMode, bool verboseMode) {
    resetSimulation();
    runSimulation(
        "SRTN", 
        [this](int currentTime) { return selectShortestRemainingTimeNext(currentTime); },
        true,  // preemptive
        0,     // no time quantum
        detailedMode,
        verboseMode
    );
}

void Simulator::runRR(int timeQuantum, bool detailedMode, bool verboseMode) {
    resetSimulation();
    runSimulation(
        "RR" + std::to_string(timeQuantum), 
        [this](int currentTime) { return selectRoundRobin(currentTime); },
        true,  // preemptive
        timeQuantum,
        detailedMode,
        verboseMode
    );
}

void Simulator::runSimulation(
    const std::string& algorithm, 
    std::function<Process*(int)> selectNextProcess,
    bool isPreemptive,
    int timeQuantum,
    bool detailedMode, 
    bool verboseMode
) {
    // Schedule process arrival events
    for (auto& process : processes) {
        eventQueue.emplace(process.getArrivalTime(), &process, EventType::PROCESS_ARRIVAL);
    }

    int currentTime = 0;
    cpuBusyTime = 0;
    
    // Main simulation loop
    while (!eventQueue.empty()) {
        // Get the next event
        Event event = eventQueue.top();
        eventQueue.pop();
        
        // Update current time
        currentTime = event.getTime();
        Process* process = event.getProcess();
        
        // Handle the event based on its type
        switch (event.getType()) {
            case EventType::PROCESS_ARRIVAL:
                handleProcessArrival(process, currentTime, verboseMode);
                break;
                
            case EventType::CPU_BURST_COMPLETION:
                handleCpuBurstCompletion(process, currentTime, verboseMode);
                break;
                
            case EventType::IO_BURST_COMPLETION:
                handleIoBurstCompletion(process, currentTime, verboseMode);
                break;
                
            case EventType::TIME_SLICE_EXPIRATION:
                handleTimeSliceExpiration(process, currentTime, verboseMode);
                break;
                
            case EventType::PROCESS_TERMINATED:
                // Already handled in CPU burst completion
                break;
        }

        // Check if we need to schedule a new process on the CPU
        if (runningProcess == nullptr && !readyQueue.empty()) {
            // Advance time for process switch overhead if needed
            if (nextProcessTime > currentTime) {
                currentTime = nextProcessTime;
            }
            
            // Select next process to run
            Process* nextProcess = selectNextProcess(currentTime);
            
            if (nextProcess != nullptr) {
                nextProcess->setState(ProcessState::RUNNING);
                nextProcess->setLastStateChangeTime(currentTime);
                runningProcess = nextProcess;
                
                if (verboseMode) {
                    std::cout << "At time " << currentTime << ": Process " 
                              << nextProcess->getId() << " moves from " 
                              << processStateToString(ProcessState::READY) << " to " 
                              << processStateToString(ProcessState::RUNNING) << std::endl;
                }
                
                // Schedule CPU burst completion event
                Burst& burst = nextProcess->getCurrentBurst();
                int completionTime = currentTime + burst.remainingTime;
                
                // If using RR, check if time slice will expire before burst completes
                if (timeQuantum > 0 && burst.remainingTime > timeQuantum) {
                    // Schedule time slice expiration
                    eventQueue.emplace(currentTime + timeQuantum, nextProcess, EventType::TIME_SLICE_EXPIRATION);
                } else {
                    // Schedule CPU burst completion
                    eventQueue.emplace(completionTime, nextProcess, EventType::CPU_BURST_COMPLETION);
                }
            }
        }
        
        // Check if preemption is needed for SRTN
        if (isPreemptive && runningProcess != nullptr && !readyQueue.empty() && algorithm == "SRTN") {
            Process* shortestProcess = selectShortestRemainingTimeNext(currentTime);
            
            if (shortestProcess != nullptr && shortestProcess != runningProcess) {
                // Preempt the currently running process
                Burst& runningBurst = runningProcess->getCurrentBurst();
                int elapsedTime = currentTime - runningProcess->getLastStateChangeTime();
                runningBurst.remainingTime -= elapsedTime;
                
                runningProcess->updateServiceTime(currentTime);
                runningProcess->setState(ProcessState::READY);
                runningProcess->setLastStateChangeTime(currentTime);
                
                if (verboseMode) {
                    std::cout << "At time " << currentTime << ": Process " 
                              << runningProcess->getId() << " moves from " 
                              << processStateToString(ProcessState::RUNNING) << " to " 
                              << processStateToString(ProcessState::READY) << " (preempted)" << std::endl;
                }
                
                // Remove the current CPU burst completion event
                std::priority_queue<Event, std::vector<Event>, EventComparator> tempQueue;
                while (!eventQueue.empty()) {
                    Event e = eventQueue.top();
                    eventQueue.pop();
                    
                    if (e.getProcess() != runningProcess || 
                        (e.getType() != EventType::CPU_BURST_COMPLETION && 
                         e.getType() != EventType::TIME_SLICE_EXPIRATION)) {
                        tempQueue.push(e);
                    }
                }
                eventQueue = tempQueue;
                
                // Add the preempted process back to the ready queue
                readyQueue.push_back(runningProcess);
                runningProcess = nullptr;
                
                // Schedule next process after overhead
                nextProcessTime = currentTime + processSwitchOverhead;
                
                // Process the next event
                continue;
            }
        }
    }
    
    // Calculate CPU utilization and total time
    double cpuUtilization = static_cast<double>(cpuBusyTime) / currentTime;
    
    // Output statistics
    Statistics stats(algorithm, currentTime, cpuUtilization, processes);
    stats.output(detailedMode);
}

void Simulator::handleProcessArrival(Process* process, int currentTime, bool verboseMode) {
    if (verboseMode) {
        std::cout << "At time " << currentTime << ": Process " 
                  << process->getId() << " moves from " 
                  << processStateToString(ProcessState::NEW) << " to " 
                  << processStateToString(ProcessState::READY) << std::endl;
    }
    
    process->setState(ProcessState::READY);
    process->setLastStateChangeTime(currentTime);
    readyQueue.push_back(process);
}

void Simulator::handleCpuBurstCompletion(Process* process, int currentTime, bool verboseMode) {
    // Update process stats
    process->updateServiceTime(currentTime);
    cpuBusyTime += (currentTime - process->getLastStateChangeTime());
    
    process->moveToNextBurst();
    
    // Check if process has completed all bursts
    if (process->isCompleted()) {
        if (verboseMode) {
            std::cout << "At time " << currentTime << ": Process " 
                      << process->getId() << " moves from " 
                      << processStateToString(ProcessState::RUNNING) << " to " 
                      << processStateToString(ProcessState::TERMINATED) << std::endl;
        }
        
        process->setState(ProcessState::TERMINATED);
        process->setFinishTime(currentTime);
        
        // Remove from running
        runningProcess = nullptr;
        
        // Set next process time with overhead
        nextProcessTime = currentTime + processSwitchOverhead;
    } else {
        // Process still has more bursts
        if (verboseMode) {
            std::cout << "At time " << currentTime << ": Process " 
                      << process->getId() << " moves from " 
                      << processStateToString(ProcessState::RUNNING) << " to " 
                      << processStateToString(ProcessState::BLOCKED) << std::endl;
        }
        
        process->setState(ProcessState::BLOCKED);
        process->setLastStateChangeTime(currentTime);
        
        // Schedule IO completion
        Burst& ioBurst = process->getCurrentBurst();
        int ioCompletionTime = currentTime + ioBurst.duration;
        eventQueue.emplace(ioCompletionTime, process, EventType::IO_BURST_COMPLETION);
        
        // Remove from running
        runningProcess = nullptr;
        
        // Set next process time with overhead
        nextProcessTime = currentTime + processSwitchOverhead;
    }
}

void Simulator::handleIoBurstCompletion(Process* process, int currentTime, bool verboseMode) {
    // Update IO time
    process->updateIOTime(currentTime);
    
    // Move to next burst (CPU burst)
    process->moveToNextBurst();
    
    if (verboseMode) {
        std::cout << "At time " << currentTime << ": Process " 
                  << process->getId() << " moves from " 
                  << processStateToString(ProcessState::BLOCKED) << " to " 
                  << processStateToString(ProcessState::READY) << std::endl;
    }
    
    // Update process state
    process->setState(ProcessState::READY);
    process->setLastStateChangeTime(currentTime);
    
    // Add to ready queue
    readyQueue.push_back(process);
}

void Simulator::handleTimeSliceExpiration(Process* process, int currentTime, bool verboseMode) {
    // Process is still running, preempt it
    if (runningProcess == process && process->getState() == ProcessState::RUNNING) {
        // Update process stats
        process->updateServiceTime(currentTime);
        cpuBusyTime += (currentTime - process->getLastStateChangeTime());
        
        // Update remaining time for current burst
        Burst& burst = process->getCurrentBurst();
        int elapsedTime = currentTime - process->getLastStateChangeTime();
        burst.remainingTime -= elapsedTime;
        
        if (verboseMode) {
            std::cout << "At time " << currentTime << ": Process " 
                      << process->getId() << " moves from " 
                      << processStateToString(ProcessState::RUNNING) << " to " 
                      << processStateToString(ProcessState::READY) << " (time slice expired)" << std::endl;
        }
        
        // Update process state
        process->setState(ProcessState::READY);
        process->setLastStateChangeTime(currentTime);
        
        // Add to ready queue
        readyQueue.push_back(process);
        
        // Remove from running
        runningProcess = nullptr;
        
        // Set next process time with overhead
        nextProcessTime = currentTime + processSwitchOverhead;
    }
}

Process* Simulator::selectFirstComeFirstServe(int currentTime) {
    if (readyQueue.empty()) {
        return nullptr;
    }
    
    // Find the process that arrived first
    auto minArrivalIt = std::min_element(readyQueue.begin(), readyQueue.end(),
        [](Process* a, Process* b) {
            return a->getArrivalTime() < b->getArrivalTime();
        });
    
    Process* selectedProcess = *minArrivalIt;
    readyQueue.erase(minArrivalIt);
    
    return selectedProcess;
}

Process* Simulator::selectShortestJobFirst(int currentTime) {
    if (readyQueue.empty()) {
        return nullptr;
    }
    
    // Find the process with the shortest current CPU burst
    auto shortestBurstIt = std::min_element(readyQueue.begin(), readyQueue.end(),
        [](Process* a, Process* b) {
            return a->getCurrentBurst().duration < b->getCurrentBurst().duration;
        });
    
    Process* selectedProcess = *shortestBurstIt;
    readyQueue.erase(shortestBurstIt);
    
    return selectedProcess;
}

Process* Simulator::selectShortestRemainingTimeNext(int currentTime) {
    if (readyQueue.empty()) {
        return nullptr;
    }
    
    // Find the process with the shortest remaining time in current CPU burst
    auto shortestRemainingIt = std::min_element(readyQueue.begin(), readyQueue.end(),
        [](Process* a, Process* b) {
            return a->getCurrentBurst().remainingTime < b->getCurrentBurst().remainingTime;
        });
    
    Process* selectedProcess = *shortestRemainingIt;
    readyQueue.erase(shortestRemainingIt);
    
    return selectedProcess;
}

Process* Simulator::selectRoundRobin(int currentTime) {
    if (readyQueue.empty()) {
        return nullptr;
    }
    
    // Round-robin simply takes the next process in the queue
    Process* selectedProcess = readyQueue.front();
    readyQueue.erase(readyQueue.begin());
    
    return selectedProcess;
}

void Simulator::resetSimulation() {
    // Clear event and ready queues
    while (!eventQueue.empty()) {
        eventQueue.pop();
    }
    readyQueue.clear();
    
    // Reset processes
    for (auto& process : processes) {
        process.reset();
    }
    
    // Reset simulator state
    runningProcess = nullptr;
    lastReadyQueueIndex = 0;
    nextProcessTime = 0;
    cpuBusyTime = 0;
}