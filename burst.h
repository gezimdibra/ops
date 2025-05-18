#ifndef BURST_H
#define BURST_H

#include "types.h"

// Structure to represent a CPU or IO burst
class Burst {
public:
    Burst(BurstType type, int duration);
    
    BurstType type;
    int duration;
    int remainingTime;
    
    // Reset the burst to its initial state
    void reset();
};

#endif // BURST_H