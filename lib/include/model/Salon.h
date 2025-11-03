// lib/include/model/Salon.h
#pragma once
#include <string>
#include <vector>
#include "model/Employee.h"
#include "model/Service.h"
#include "model/Appointment.h"
#include "model/TimeSlot.h"

class Salon {
public:
    explicit Salon(std::string name = {}) : m_name(std::move(name)) {}
    const std::string& getName() const noexcept { return m_name; }

    void setWorkingHours(const TimeSlot& t) { m_workingHours = t; }
    const TimeSlot& getWorkingHours() const noexcept { return m_workingHours; }

    void addEmployee(const Employee& e) { m_employees.push_back(e); }
    void addService(const Service& s)   { m_services.push_back(s); }
    void addAppointment(const Appointment& a) { m_appointments.push_back(a); }

    // yeni: randevu silme
    bool removeAppointmentAt(std::size_t index) {
        if (index >= m_appointments.size()) return false;
        m_appointments.erase(m_appointments.begin() + static_cast<std::ptrdiff_t>(index));
        return true;
    }

    const std::vector<Employee>&    getEmployees() const noexcept { return m_employees; }
    const std::vector<Service>&     getServices()  const noexcept { return m_services; }
    const std::vector<Appointment>& getAppointments() const noexcept { return m_appointments; }

private:
    std::string m_name;
    TimeSlot m_workingHours{};
    std::vector<Employee>    m_employees;
    std::vector<Service>     m_services;
    std::vector<Appointment> m_appointments;
};
