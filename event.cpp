#include "event.h"

Event::Event(int time, Process* process, EventType type)
    : time(time), process(process), type(type) {}

int Event::getTime() const {
    return time;
}

Process* Event::getProcess() const {
    return process;
}

EventType Event::getType() const {
    return type;
}

bool Event::operator<(const Event& other) const {
    if (time == other.time) {
        // If times are equal, prioritize by event type
        return static_cast<int>(type) < static_cast<int>(other.type);
    }
    return time < other.time;
}

bool Event::operator>(const Event& other) const {
    if (time == other.time) {
        // If times are equal, prioritize by event type
        return static_cast<int>(type) > static_cast<int>(other.type);
    }
    return time > other.time;
}