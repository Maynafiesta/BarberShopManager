// lib/include/model/TimeSlot.h
#pragma once
#include <ctime>

struct TimeSlot {
    std::time_t startEpoch{};
    int durationMinutes{0};

    bool overlaps(const TimeSlot& other) const;
};
