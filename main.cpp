#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "simulator.h"
#include "process.h"
#include "generator.h"

void printUsage() {
    std::cout << "Usage: sim [-d] [-v] [-a algorithm] < input_file" << std::endl;
    std::cout << "  -d: Detailed information mode" << std::endl;
    std::cout << "  -v: Verbose mode" << std::endl;
    std::cout << "  -a algorithm: Execute a specific algorithm (FCFS, SJF, SRTN, RR10, RR50, or RR100)" << std::endl;
}

int main(int argc, char* argv[]) {
    bool detailedMode = false;
    bool verboseMode = false;
    std::string algorithm = "";

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-d") {
            detailedMode = true;
        } else if (arg == "-v") {
            verboseMode = true;
        } else if (arg == "-a" && i + 1 < argc) {
            algorithm = argv[++i];
            if (algorithm != "FCFS" && algorithm != "SJF" && algorithm != "SRTN" && 
                algorithm != "RR10" && algorithm != "RR50" && algorithm != "RR100") {
                std::cerr << "Invalid algorithm: " << algorithm << std::endl;
                printUsage();
                return 1;
            }
        } else {
            std::cerr << "Invalid argument: " << arg << std::endl;
            printUsage();
            return 1;
        }
    }

    // Create a processes vector to store the parsed processes
    std::vector<Process> processes;
    int processSwitchOverhead = 0;

    // Read input from stdin
    std::string line;
    int lineNum = 0;
    int numProcesses = 0;
    int currentProcess = 0;
    int currentBurst = 0;
    int maxBursts = 0;

    while (std::getline(std::cin, line)) {
        if (lineNum == 0) {
            // First line: number of processes and process switch overhead
            std::istringstream iss(line);
            iss >> numProcesses >> processSwitchOverhead;
            lineNum++;
        } else {
            std::istringstream iss(line);
            int value1, value2, value3;

            if (iss >> value1 >> value2 >> value3) {
                if (currentBurst == 0) {
                    // This is a process definition line (process number, arrival time, number of bursts)
                    currentProcess = value1;
                    int arrivalTime = value2;
                    maxBursts = value3;
                    
                    // Create a new process
                    Process process(currentProcess, arrivalTime);
                    processes.push_back(process);
                    currentBurst = 1;
                } else {
                    // This is a burst definition line (burst number, CPU time, IO time)
                    int burstNum = value1;
                    int cpuTime = value2;
                    int ioTime = value3;
                    
                    // Add CPU burst
                    processes.back().addCPUBurst(cpuTime);
                    
                    // Add IO burst if this is not the last burst
                    if (burstNum < maxBursts) {
                        processes.back().addIOBurst(ioTime);
                    }
                    
                    currentBurst++;
                    if (currentBurst > maxBursts) {
                        // Move to the next process
                        currentBurst = 0;
                    }
                }
            } else {
                std::cerr << "Error parsing line: " << line << std::endl;
                return 1;
            }
        }
    }

    // If no processes were read, generate random processes
    if (processes.empty()) {
        std::cout << "No input provided. Generating 50 random processes..." << std::endl;
        ProcessGenerator generator;
        processes = generator.generateProcesses(50, 5);
        processSwitchOverhead = 5;
        
        // Save the generated processes to a file for reference
        std::ofstream outFile("generated_processes.txt");
        outFile << processes.size() << " " << processSwitchOverhead << std::endl;
        for (const auto& process : processes) {
            outFile << process.getId() << " " << process.getArrivalTime() << " " 
                    << process.getBursts().size() / 2 + 1 << std::endl;
            
            for (size_t i = 0; i < process.getBursts().size(); ++i) {
                const Burst& burst = process.getBursts()[i];
                if (burst.type == BurstType::CPU) {
                    int burstNum = (i / 2) + 1;
                    outFile << burstNum << " " << burst.duration;
                    
                    // If there's an IO burst after this CPU burst, add it
                    if (i + 1 < process.getBursts().size()) {
                        outFile << " " << process.getBursts()[i + 1].duration << std::endl;
                    } else {
                        outFile << std::endl;
                    }
                }
            }
        }
        outFile.close();
        std::cout << "Random processes saved to 'generated_processes.txt'" << std::endl;
    }

    // Create simulator
    Simulator simulator(processes, processSwitchOverhead);

    // Run simulations based on algorithm flag
    if (algorithm.empty() || algorithm == "FCFS") {
        simulator.runFCFS(detailedMode, verboseMode);
    }
    
    if (algorithm.empty() || algorithm == "SJF") {
        simulator.runSJF(detailedMode, verboseMode);
    }
    
    if (algorithm.empty() || algorithm == "SRTN") {
        simulator.runSRTN(detailedMode, verboseMode);
    }
    
    if (algorithm.empty() || algorithm == "RR10") {
        simulator.runRR(10, detailedMode, verboseMode);
    }
    
    if (algorithm.empty() || algorithm == "RR50") {
        simulator.runRR(50, detailedMode, verboseMode);
    }
    
    if (algorithm.empty() || algorithm == "RR100") {
        simulator.runRR(100, detailedMode, verboseMode);
    }

    return 0;
}
