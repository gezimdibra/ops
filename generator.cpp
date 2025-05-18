#include <ctime>
#include <cmath>
#include "generator.h"

ProcessGenerator::ProcessGenerator()
    : rng(std::random_device{}()),
      arrivalDist(1.0 / 50.0),  // Mean of 50 time units
      cpuBurstDist(5, 500),     // 5 to 500 time units
      ioBurstDist(30, 1000),    // 30 to 1000 time units
      burstCountDist(20.0, 5.0) // Mean of 20 bursts with standard deviation of 5
{}

std::vector<Process> ProcessGenerator::generateProcesses(int count, int processSwitchOverhead) {
    std::vector<Process> processes;
    
    int currentArrivalTime = 0;
    
    for (int i = 1; i <= count; ++i) {
        // Generate arrival time
        currentArrivalTime += generateArrivalTime(currentArrivalTime);
        
        // Create the process
        Process process(i, currentArrivalTime);
        
        // Generate bursts
        int burstCount = generateBurstCount();
        
        // Ensure odd number of bursts (ending with CPU)
        if (burstCount % 2 == 0) {
            burstCount++;
        }
        
        // Add CPU and IO bursts alternating
        for (int j = 0; j < burstCount; ++j) {
            if (j % 2 == 0) {
                // CPU burst
                process.addCPUBurst(generateCPUBurst());
            } else {
                // IO burst
                process.addIOBurst(generateIOBurst());
            }
        }
        
        processes.push_back(process);
    }
    
    return processes;
}

int ProcessGenerator::generateArrivalTime(int previousArrival) {
    // Generate next interarrival time using exponential distribution
    return static_cast<int>(arrivalDist(rng));
}

int ProcessGenerator::generateCPUBurst() {
    return cpuBurstDist(rng);
}

int ProcessGenerator::generateIOBurst() {
    return ioBurstDist(rng);
}

int ProcessGenerator::generateBurstCount() {
    // Generate number of bursts with normal distribution, but ensure minimum of 1
    int count = std::max(1, static_cast<int>(std::round(burstCountDist(rng))));
    
    // Ensure odd number (ends with CPU burst)
    if (count % 2 == 0) {
        count++;
    }
    
    return count;
}