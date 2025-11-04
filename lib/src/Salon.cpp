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
