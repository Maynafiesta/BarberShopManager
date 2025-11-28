// lib/src/Salon.cpp
#include "model/Salon.h"

bool Salon::rejectAppointmentAt(size_t idx) {
    if (idx >= m_appointments.size()) return false;
    m_appointments[idx].reject();
    return true;
}

bool Salon::removeAppointmentAt(size_t idx) {
    if (idx >= m_appointments.size()) return false;
    m_appointments.erase(m_appointments.begin() + static_cast<long>(idx));
    return true;
}

bool Salon::addSkillToEmployee(size_t idx, const std::string& skill) {
    if (idx >= m_employees.size()) return false;
    if (!m_employees[idx].hasSkill(skill))
        m_employees[idx].addSkill(skill);
    return true;
}

bool Salon::addAvailabilityToEmployee(size_t idx, const TimeSlot& slot) {
    if (idx >= m_employees.size()) return false;
    m_employees[idx].addAvailability(slot);
    return true;
}
