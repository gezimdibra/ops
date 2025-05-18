# CPU Scheduling Algorithm Simulator

This program simulates and evaluates the performance of different CPU scheduling algorithms:

1. First Come First Serve (FCFS)
2. Shortest Job First (SJF) - Non-preemptive
3. Shortest Remaining Time Next (SRTN) - Preemptive 
4. Round Robin (RR) - With time quanta of 10, 50, and 100

## Compilation

To compile the program, use the provided Makefile:

```
cd project
make
```

This will create the `sim` executable.

## Usage

The simulator can be invoked with the following command:

```
./sim [-d] [-v] [-a algorithm] < input_file
```

Where:
- `-d` enables detailed information mode
- `-v` enables verbose mode
- `-a algorithm` specifies which algorithm to run (FCFS, SJF, SRTN, RR10, RR50, or RR100)

Input is provided through standard input (using redirection `<`).

## Input Format

The input file format is as follows:

```
number_of_processes process_switch_overhead
process_number arrival_time number_of_bursts
1 cpu_time io_time
2 cpu_time io_time
...
number_of_bursts cpu_time
process_number arrival_time number_of_bursts
...
```

If no input is provided, the program will automatically generate 50 random processes.

## Output Format

### Default Mode
Shows the total time required and CPU utilization for each algorithm.

### Detailed Mode (-d)
Shows detailed statistics including the total time, CPU utilization, and per-process statistics (arrival time, service time, I/O time, turnaround time, and finish time).

### Verbose Mode (-v)
Shows all process state transitions during the simulation.

## Example

```
./sim -d -a FCFS < input_file
```

This will run the First Come First Serve algorithm with detailed output.