// lib/include/core/Scheduler.h
#pragma once
#include "model/Salon.h"
#include "model/Appointment.h"

class Scheduler {
public:
    bool createAppointment(Salon& salon,
                           const Customer& customer,
                           const Employee& employee,
                           const Service& service,
                           const TimeSlot& desiredSlot,
                           Appointment& outAppointment);

private:
    bool isEmployeeAvailable(const Employee& employee, const TimeSlot& desiredSlot) const;
    bool isInsideWorkingHours(const Salon& salon, const TimeSlot& desiredSlot) const;
    bool hasCollision(const Salon& salon, const Employee& employee, const TimeSlot& desiredSlot) const;
};
