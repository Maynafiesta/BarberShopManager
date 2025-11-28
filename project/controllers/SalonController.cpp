// project/controllers/SalonController.cpp
#include "controllers/SalonController.h"

#include <QDateTime>
#include <QTime>
#include <algorithm>

namespace {
    TimeSlot slotFor(const QDate& day, int hour, int minute, int durationMinutes) {
        return TimeSlot{ static_cast<std::time_t>(
            QDateTime(day, QTime(hour, minute), Qt::LocalTime).toSecsSinceEpoch()), durationMinutes };
    }
}

Salon& SalonController::active() {
    return m_salons.at(m_activeSalon);
}

const Salon& SalonController::active() const {
    return m_salons.at(m_activeSalon);
}

Salon* SalonController::currentSalon() {
    if (m_salons.empty()) return nullptr;
    return &m_salons.at(m_activeSalon);
}

const Salon* SalonController::currentSalon() const {
    if (m_salons.empty()) return nullptr;
    return &m_salons.at(m_activeSalon);
}

const std::vector<Employee>& SalonController::employees() const {
    static std::vector<Employee> empty;
    return m_salons.empty() ? empty : active().getEmployees();
}

const std::vector<Service>& SalonController::services() const {
    static std::vector<Service> empty;
    return m_salons.empty() ? empty : active().getServices();
}

const std::vector<Appointment>& SalonController::appointments() const {
    static std::vector<Appointment> empty;
    return m_salons.empty() ? empty : active().getAppointments();
}

bool SalonController::setActiveSalon(size_t idx) {
    if (idx >= m_salons.size()) return false;
    m_activeSalon = idx;
    return true;
}

bool SalonController::addSalon(const std::string& name, const TimeSlot& workingHours) {
    const auto it = std::find_if(m_salons.begin(), m_salons.end(), [&](const Salon& s) {
        return s.getName() == name;
    });
    if (it != m_salons.end()) return false;

    Salon s(name);
    s.setWorkingHours(workingHours);
    m_salons.push_back(s);
    m_activeSalon = m_salons.size() - 1;
    return true;
}

bool SalonController::addEmployeeToActive(const Employee& e) {
    if (m_salons.empty()) return false;
    active().addEmployee(e);
    return true;
}

bool SalonController::addEmployeeToSalon(size_t salonIdx, const Employee& e) {
    if (salonIdx >= m_salons.size()) return false;
    m_salons[salonIdx].addEmployee(e);
    return true;
}

bool SalonController::addServiceToActive(const Service& s) {
    if (m_salons.empty()) return false;
    active().addService(s);
    return true;
}

bool SalonController::addSkillToEmployee(size_t employeeIdx, const std::string& skill) {
    if (m_salons.empty()) return false;
    return active().addSkillToEmployee(employeeIdx, skill);
}

bool SalonController::addAvailabilityToEmployee(size_t employeeIdx, const TimeSlot& slot) {
    if (m_salons.empty()) return false;
    return active().addAvailabilityToEmployee(employeeIdx, slot);
}

bool SalonController::rejectAppointmentAt(size_t idx) {
    if (m_salons.empty()) return false;
    return active().rejectAppointmentAt(idx);
}

bool SalonController::removeAppointmentAt(size_t idx) {
    if (m_salons.empty()) return false;
    return active().removeAppointmentAt(idx);
}

void SalonController::loadDemoData(const QDate& day) {
    m_salons.clear();

    // Merkez şube: 09:00 - 21:00
    Salon merkez("Merkez Şube");
    merkez.setWorkingHours(slotFor(day, 9, 0, 12 * 60));

    Employee e1("Ahmet Usta", "0500 000 0001");
    e1.addSkill("Saç kesimi");
    e1.addSkill("Sakal tıraşı");
    e1.addAvailability(slotFor(day, 10, 0, 4 * 60));

    Employee e2("Merve", "0500 000 0002");
    e2.addSkill("Boya");
    e2.addSkill("Fön");
    e2.addAvailability(slotFor(day, 12, 0, 6 * 60));

    merkez.addEmployee(e1);
    merkez.addEmployee(e2);

    merkez.addService(Service("Saç kesimi", 30, 250.0));
    merkez.addService(Service("Sakal tıraşı", 20, 150.0));
    merkez.addService(Service("Boya", 90, 900.0));
    merkez.addService(Service("Fön", 15, 120.0));

    // İkinci şube: farklı mesai ve uzmanlıklar
    Salon ikinci("Şube 2");
    ikinci.setWorkingHours(slotFor(day, 8, 30, 10 * 60)); // 08:30 - 18:30

    Employee e3("Burak", "0500 000 0003");
    e3.addSkill("Saç kesimi");
    e3.addSkill("Çocuk tıraşı");
    e3.addAvailability(slotFor(day, 9, 0, 5 * 60));

    Employee e4("Elif", "0500 000 0004");
    e4.addSkill("Boya");
    e4.addSkill("Fön");
    e4.addSkill("Keratin bakımı");
    e4.addAvailability(slotFor(day, 13, 0, 5 * 60));

    ikinci.addEmployee(e3);
    ikinci.addEmployee(e4);

    ikinci.addService(Service("Çocuk tıraşı", 25, 180.0));
    ikinci.addService(Service("Saç kesimi", 30, 230.0));
    ikinci.addService(Service("Keratin bakımı", 75, 1200.0));
    ikinci.addService(Service("Boya", 80, 850.0));

    m_salons.push_back(merkez);
    m_salons.push_back(ikinci);

    m_activeSalon = 0;

    m_customers.clear();
    m_customers.emplace_back("Tarık", "0555 555 55 55");
}
