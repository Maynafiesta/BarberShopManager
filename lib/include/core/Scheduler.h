#pragma once
#include "Salon.h"
#include "Appointment.h"
#include "TimeSlot.h"
#include "Service.h"
#include "Customer.h"
#include "Employee.h"

class Scheduler {
public:
    // randevu yaratmaya çalışır
    // başarılıysa Appointment döner ve salon'a ekler
    // başarısızsa nullptr döner gibi tasarlayabiliriz;
    // ama ham pointer return etmektense bool + out param daha güvenli.
    bool createAppointment(Salon& salon,
                           const Customer& customer,
                           const Employee& employee,
                           const Service& service,
                           const TimeSlot& desiredSlot,
                           Appointment& outAppointment);

private:
    bool isEmployeeAvailable(const Employee& employee,
                             const TimeSlot& desiredSlot) const;

    bool isInsideWorkingHours(const Salon& salon,
                              const TimeSlot& desiredSlot) const;

    bool hasCollision(const Salon& salon,
                      const Employee& employee,
                      const TimeSlot& desiredSlot) const;
};
