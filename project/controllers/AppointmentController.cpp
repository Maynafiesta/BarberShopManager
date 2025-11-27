// project/controllers/AppointmentController.cpp
#include "controllers/AppointmentController.h"

Scheduler::CreateResult AppointmentController::create(const Customer& customer,
                                                      const Employee& employee,
                                                      const Service& service,
                                                      const TimeSlot& desired,
                                                      Appointment& out) {
    if (!m_salon) return Scheduler::CreateResult::OutsideWorkingHours;
    return m_scheduler.createAppointment(*m_salon, customer, employee, service, desired, out);
}

const std::vector<Appointment>& AppointmentController::list() const {
    static std::vector<Appointment> empty;
    return m_salon ? m_salon->getAppointments() : empty;
}
