#include "burst.h"

Burst::Burst(BurstType type, int duration) 
    : type(type), duration(duration), remainingTime(duration) {}

void Burst::reset() {
    remainingTime = duration;
}