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

bool Scheduler::createAppointment(Salon& salon,
                                  const Customer& customer,
                                  const Employee& employee,
                                  const Service& service,
                                  const TimeSlot& desired,
                                  Appointment& out) {
    if (!isInsideWorkingHours(salon, desired)) return false;
    if (!isEmployeeAvailable(employee, desired)) return false;
    if (hasCollision(salon, employee, desired))  return false;

    out = Appointment(&customer, &employee, service, desired);
    // onay sürecini şimdilik otomatik yapalım
    const_cast<Appointment&>(out).approve();
    salon.addAppointment(out);
    return true;
}
