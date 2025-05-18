#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>
#include <queue>
#include <string>
#include <functional>
#include "process.h"
#include "event.h"

// Custom comparator for event priority queue
struct EventComparator {
    bool operator()(const Event& a, const Event& b) const {
        return a > b;
    }
};

class Simulator {
public:
    Simulator(std::vector<Process>& processes, int processSwitchOverhead);
    
    // Run different scheduling algorithms
    void runFCFS(bool detailedMode, bool verboseMode);
    void runSJF(bool detailedMode, bool verboseMode);
    void runSRTN(bool detailedMode, bool verboseMode);
    void runRR(int timeQuantum, bool detailedMode, bool verboseMode);
    
private:
    std::vector<Process> processes;
    int processSwitchOverhead;
    
    // Core simulation method
    void runSimulation(
        const std::string& algorithm, 
        std::function<Process*(int)> selectNextProcess,
        bool isPreemptive,
        int timeQuantum,
        bool detailedMode, 
        bool verboseMode
    );
    
    // Process state transition helpers
    void handleProcessArrival(Process* process, int currentTime, bool verboseMode);
    void handleCpuBurstCompletion(Process* process, int currentTime, bool verboseMode);
    void handleIoBurstCompletion(Process* process, int currentTime, bool verboseMode);
    void handleTimeSliceExpiration(Process* process, int currentTime, bool verboseMode);
    
    // Process selection strategies
    Process* selectFirstComeFirstServe(int currentTime);
    Process* selectShortestJobFirst(int currentTime);
    Process* selectShortestRemainingTimeNext(int currentTime);
    Process* selectRoundRobin(int currentTime);
    
    // Reset simulation state
    void resetSimulation();
    
    // Event queue and ready queue
    std::priority_queue<Event, std::vector<Event>, EventComparator> eventQueue;
    std::vector<Process*> readyQueue;
    
    // Simulation state
    Process* runningProcess;
    int lastReadyQueueIndex;
    int nextProcessTime;
    int cpuBusyTime;
};

#endif // SIMULATOR_H