#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>
#include <string>
#include "process.h"

class Statistics {
public:
    Statistics(const std::string& algorithm, int totalTime, 
               double cpuUtilization, const std::vector<Process>& processes);
    
    // Output statistics based on mode
    void output(bool detailedMode) const;
    
private:
    std::string algorithm;
    int totalTime;
    double cpuUtilization;
    std::vector<Process> processes;
    
    // Helper methods for formatting output
    void outputDefault() const;
    void outputDetailed() const;
};

#endif // STATISTICS_H