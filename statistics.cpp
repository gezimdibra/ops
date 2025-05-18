#include <iostream>
#include <iomanip>
#include "statistics.h"
#include "types.h"

Statistics::Statistics(const std::string& algorithm, int totalTime, 
                      double cpuUtilization, const std::vector<Process>& processes)
    : algorithm(algorithm), 
      totalTime(totalTime), 
      cpuUtilization(cpuUtilization), 
      processes(processes) {}

void Statistics::output(bool detailedMode) const {
    if (detailedMode) {
        outputDetailed();
    } else {
        outputDefault();
    }
}

void Statistics::outputDefault() const {
    std::cout << algorithmToString(algorithm) << ": ";
    std::cout << "Total Time required is " << totalTime << " time units ";
    std::cout << "CPU Utilization is " << std::fixed << std::setprecision(0) 
              << (cpuUtilization * 100) << "%" << std::endl;
}

void Statistics::outputDetailed() const {
    std::cout << algorithmToString(algorithm) << ": ";
    std::cout << "Total Time required is " << totalTime << " units ";
    std::cout << "CPU Utilization is " << std::fixed << std::setprecision(0) 
              << (cpuUtilization * 100) << "%" << std::endl;
    
    for (const auto& process : processes) {
        std::cout << "Process " << process.getId() << ":" << std::endl;
        std::cout << "  arrival time: " << process.getArrivalTime() << " units" << std::endl;
        std::cout << "  service time: " << process.getTotalServiceTime() << " units" << std::endl;
        std::cout << "  I/O time: " << process.getTotalIOTime() << " units" << std::endl;
        std::cout << "  turnaround time: " << process.getTurnaroundTime() << " units" << std::endl;
        std::cout << "  finish time: " << process.getFinishTime() << " units" << std::endl;
    }
}