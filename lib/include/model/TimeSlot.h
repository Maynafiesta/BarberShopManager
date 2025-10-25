#pragma once
#include <ctime>

struct TimeSlot {
    // basit tutalım: epoch start + durationDakika
    std::time_t startEpoch;
    int durationMinutes;

    bool overlaps(const TimeSlot& other) const;
};
