// project/controllers/AppointmentController.h
#pragma once
#include "core/Scheduler.h"

class AppointmentController {
public:
    explicit AppointmentController(Scheduler& scheduler)
        : m_scheduler(scheduler) {}

    void setActiveSalon(Salon* salon) { m_salon = salon; }

    Scheduler::CreateResult create(const Customer& customer,
                                   const Employee& employee,
                                   const Service& service,
                                   const TimeSlot& desiredSlot,
                                   Appointment& outAppointment);

    bool reject(size_t idx) { return m_salon ? m_salon->rejectAppointmentAt(idx) : false; }
    bool remove(size_t idx) { return m_salon ? m_salon->removeAppointmentAt(idx) : false; }

    const std::vector<Appointment>& list() const;

private:
    Salon*     m_salon{nullptr};
    Scheduler& m_scheduler;
};
