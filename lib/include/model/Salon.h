#pragma once
#include <string>
#include <vector>
#include "Employee.h"
#include "Service.h"
#include "Appointment.h"
#include "TimeSlot.h"

class Salon {
public:
    Salon(std::string name)
        : m_name(std::move(name)) {}

    const std::string& getName() const noexcept { return m_name; }

    void setWorkingHours(const TimeSlot& dailyWorkingHours) {
        m_workingHours = dailyWorkingHours;
    }

    const TimeSlot& getWorkingHours() const noexcept { return m_workingHours; }

    void addEmployee(const Employee& e) {
        m_employees.push_back(e);
    }

    void addService(const Service& s) {
        m_services.push_back(s);
    }

    const std::vector<Employee>& getEmployees() const noexcept {
        return m_employees;
    }

    const std::vector<Service>& getServices() const noexcept {
        return m_services;
    }

    const std::vector<Appointment>& getAppointments() const noexcept {
        return m_appointments;
    }

    // Randevu eklemeden önce çakışma kontrolünü ayrı sınıfa bırakacağız (Scheduler)
    void addAppointment(const Appointment& app) {
        m_appointments.push_back(app);
    }

private:
    std::string m_name;
    TimeSlot m_workingHours; // örn: 09:00-21:00 (başlangıç epoch + 12 saat)

    std::vector<Employee>    m_employees;
    std::vector<Service>     m_services;
    std::vector<Appointment> m_appointments;
};
