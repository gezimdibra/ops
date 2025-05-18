#ifndef EVENT_H
#define EVENT_H

#include "process.h"
#include "types.h"

class Event {
public:
    Event(int time, Process* process, EventType type);
    
    // Getters
    int getTime() const;
    Process* getProcess() const;
    EventType getType() const;
    
    // For sorting events by time
    bool operator<(const Event& other) const;
    bool operator>(const Event& other) const;

private:
    int time;
    Process* process;
    EventType type;
};

#endif // EVENT_H