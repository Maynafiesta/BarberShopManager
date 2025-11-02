// lib/src/Scheduler.cpp
#include "core/Scheduler.h"

bool Scheduler::isEmployeeAvailable(const Employee& employee, const TimeSlot& desired) const {
    for (const auto& s : employee.getAvailability())
        if (s.overlaps(desired)) return true;
    return false;
}

bool Scheduler::isInsideWorkingHours(const Salon& salon, const TimeSlot& desired) const {
    return salon.getWorkingHours().overlaps(desired);
}

bool Scheduler::hasCollision(const Salon& salon, const Employee& employee, const TimeSlot& desired) const {
    for (const auto& a : salon.getAppointments())
        if (a.getEmployee() == &employee && a.getSlot().overlaps(desired))
            return true;
    return false;
}

bool Scheduler::employeeHasSkill(const Employee& employee, const Service& service) const {
    for (const auto& sk : employee.getSkills())
        if (sk == service.getName())
            return true;
    return false;
}

Scheduler::CreateResult Scheduler::createAppointment(Salon& salon,
                                                     const Customer& customer,
                                                     const Employee& employee,
                                                     const Service& service,
                                                     const TimeSlot& desired,
                                                     Appointment& out) {
    if (!isInsideWorkingHours(salon, desired))
        return CreateResult::OutsideWorkingHours;

    if (!employeeHasSkill(employee, service))
        return CreateResult::EmployeeLacksSkill;

    if (!isEmployeeAvailable(employee, desired))
        return CreateResult::EmployeeNotAvailable;

    if (hasCollision(salon, employee, desired))
        return CreateResult::Collision;

    out = Appointment(&customer, &employee, service, desired);
    const_cast<Appointment&>(out).approve();
    salon.addAppointment(out);
    return CreateResult::Ok;
}
