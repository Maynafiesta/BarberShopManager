// lib/include/core/Scheduler.h
#pragma once
#include "model/Salon.h"
#include "model/Appointment.h"

class Scheduler {
public:
    enum class CreateResult {
        Ok,
        OutsideWorkingHours,
        EmployeeNotAvailable,
        EmployeeLacksSkill,   // yeni
        Collision
    };

    CreateResult createAppointment(Salon& salon,
                                   const Customer& customer,
                                   const Employee& employee,
                                   const Service& service,
                                   const TimeSlot& desiredSlot,
                                   Appointment& outAppointment);

private:
    bool isEmployeeAvailable(const Employee& employee, const TimeSlot& desiredSlot) const;
    bool isInsideWorkingHours(const Salon& salon, const TimeSlot& desiredSlot) const;
    bool hasCollision(const Salon& salon, const Employee& employee, const TimeSlot& desiredSlot) const;
    bool employeeHasSkill(const Employee& employee, const Service& service) const; // yeni
};
