#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include <random>
#include "process.h"

class ProcessGenerator {
public:
    ProcessGenerator();
    
    // Generate random processes
    std::vector<Process> generateProcesses(int count, int processSwitchOverhead);
    
private:
    std::mt19937 rng; // Mersenne Twister random number generator
    
    // Distribution functions
    std::exponential_distribution<double> arrivalDist;
    std::uniform_int_distribution<int> cpuBurstDist;
    std::uniform_int_distribution<int> ioBurstDist;
    std::normal_distribution<double> burstCountDist;
    
    // Generate exponential distribution for arrival times
    int generateArrivalTime(int previousArrival);
    
    // Generate CPU burst duration
    int generateCPUBurst();
    
    // Generate IO burst duration
    int generateIOBurst();
    
    // Generate number of bursts per process (always odd to end with CPU)
    int generateBurstCount();
};

#endif // GENERATOR_H