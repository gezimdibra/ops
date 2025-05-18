#include "process.h"

Process::Process(int id, int arrivalTime)
    : id(id), 
      arrivalTime(arrivalTime), 
      state(ProcessState::NEW), 
      currentBurstIndex(0), 
      totalServiceTime(0), 
      totalIOTime(0), 
      finishTime(-1), 
      startTime(-1), 
      lastStateChangeTime(arrivalTime) {}

void Process::addCPUBurst(int duration) {
    bursts.emplace_back(BurstType::CPU, duration);
}

void Process::addIOBurst(int duration) {
    bursts.emplace_back(BurstType::IO, duration);
}

void Process::reset() {
    state = ProcessState::NEW;
    currentBurstIndex = 0;
    totalServiceTime = 0;
    totalIOTime = 0;
    finishTime = -1;
    startTime = -1;
    lastStateChangeTime = arrivalTime;
    
    // Reset all bursts
    for (auto& burst : bursts) {
        burst.reset();
    }
}

int Process::getId() const {
    return id;
}

int Process::getArrivalTime() const {
    return arrivalTime;
}

ProcessState Process::getState() const {
    return state;
}

int Process::getCurrentBurstIndex() const {
    return currentBurstIndex;
}

Burst& Process::getCurrentBurst() {
    return bursts[currentBurstIndex];
}

const Burst& Process::getCurrentBurst() const {
    return bursts[currentBurstIndex];
}

bool Process::hasNextBurst() const {
    return currentBurstIndex + 1 < static_cast<int>(bursts.size());
}

const std::vector<Burst>& Process::getBursts() const {
    return bursts;
}

int Process::getTotalServiceTime() const {
    return totalServiceTime;
}

int Process::getTotalIOTime() const {
    return totalIOTime;
}

int Process::getFinishTime() const {
    return finishTime;
}

int Process::getStartTime() const {
    return startTime;
}

int Process::getLastStateChangeTime() const {
    return lastStateChangeTime;
}

int Process::getTurnaroundTime() const {
    if (finishTime == -1) {
        return -1;
    }
    return finishTime - arrivalTime;
}

void Process::setState(ProcessState newState) {
    state = newState;
}

void Process::setCurrentBurstIndex(int index) {
    currentBurstIndex = index;
}

void Process::setFinishTime(int time) {
    finishTime = time;
}

void Process::setStartTime(int time) {
    if (startTime == -1) {
        startTime = time;
    }
}

void Process::setLastStateChangeTime(int time) {
    lastStateChangeTime = time;
}

void Process::updateServiceTime(int time) {
    if (getCurrentBurst().type == BurstType::CPU) {
        int elapsed = time - lastStateChangeTime;
        totalServiceTime += elapsed;
    }
}

void Process::updateIOTime(int time) {
    if (getCurrentBurst().type == BurstType::IO) {
        int elapsed = time - lastStateChangeTime;
        totalIOTime += elapsed;
    }
}

bool Process::isCompleted() const {
    return currentBurstIndex >= static_cast<int>(bursts.size());
}

void Process::moveToNextBurst() {
    if (hasNextBurst()) {
        currentBurstIndex++;
    }
}