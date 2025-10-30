// lib/src/TimeSlot.cpp
#include "model/TimeSlot.h"

bool TimeSlot::overlaps(const TimeSlot& other) const {
    const std::time_t endA = startEpoch + durationMinutes * 60;
    const std::time_t endB = other.startEpoch + other.durationMinutes * 60;
    return (startEpoch < endB) && (other.startEpoch < endA);
}
