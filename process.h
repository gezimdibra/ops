#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include "burst.h"
#include "types.h"

class Process {
public:
    Process(int id, int arrivalTime);
    
    // Add CPU and IO bursts
    void addCPUBurst(int duration);
    void addIOBurst(int duration);
    
    // Reset the process to its initial state for a new simulation
    void reset();
    
    // Getters
    int getId() const;
    int getArrivalTime() const;
    ProcessState getState() const;
    int getCurrentBurstIndex() const;
    Burst& getCurrentBurst();
    const Burst& getCurrentBurst() const;
    bool hasNextBurst() const;
    const std::vector<Burst>& getBursts() const;
    int getTotalServiceTime() const;
    int getTotalIOTime() const;
    int getFinishTime() const;
    int getStartTime() const;
    int getLastStateChangeTime() const;
    int getTurnaroundTime() const;
    
    // Setters
    void setState(ProcessState state);
    void setCurrentBurstIndex(int index);
    void setFinishTime(int time);
    void setStartTime(int time);
    void setLastStateChangeTime(int time);
    
    // Calculate statistics
    void updateServiceTime(int time);
    void updateIOTime(int time);
    
    // Check if the process has completed all bursts
    bool isCompleted() const;
    
    // Move to the next burst
    void moveToNextBurst();
    
private:
    int id;
    int arrivalTime;
    ProcessState state;
    int currentBurstIndex;
    std::vector<Burst> bursts;
    
    // Statistics
    int totalServiceTime;
    int totalIOTime;
    int finishTime;
    int startTime;
    int lastStateChangeTime;
};

#endif // PROCESS_H