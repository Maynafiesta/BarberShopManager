// lib/include/model/TimeSlot.h
#pragma once
#include <ctime>

struct TimeSlot {
    std::time_t startEpoch{};
    int durationMinutes{0};

    std::time_t endEpoch() const noexcept { return startEpoch + durationMinutes * 60; }
    bool overlaps(const TimeSlot& other) const;
    bool contains(const TimeSlot& other) const;
};
