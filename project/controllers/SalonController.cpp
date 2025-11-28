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

    Salon s( name );
    s.setWorkingHours( workingHours );
    s.addService( Service( "Saç kesimi",           30, 250 ) );
    s.addService( Service( "Fön",                  15, 120 ) );
    s.addService( Service( "Saç kesimi + fön",     40, 300 ) );
    s.addService( Service( "Sakal traşı",          20, 150 ) );
    s.addService( Service( "Saç + sakal paket",    45, 380 ) );
    s.addService( Service( "Çocuk saç kesimi",     20, 200 ) );
    s.addService( Service( "Saç boyama",           60, 600 ) );
    s.addService( Service( "Keratin bakımı",       90, 1200 ) );
    s.addService( Service( "Saç bakım maskesi",    25, 250 ) );
    s.addService( Service( "Saç yıkama + şekil",   15, 120 ) );
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

bool SalonController::setEmployeeSkills(size_t employeeIdx, const std::vector<std::string>& skills) {
    if (m_salons.empty()) return false;
    return active().setEmployeeSkills(employeeIdx, skills);
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

void SalonController::loadDemoData( const QDate& day ) {
    m_salons.clear();

    // Serdivan şube: 09:00 - 21:00
    Salon serdivanStore( "Serdivan Şube" );
    serdivanStore.setWorkingHours( slotFor( day, 9, 0, 12 * 60 ) );

    Employee e1( "Ahmet Saçkeser", "0500 000 0001" );
    e1.addSkill( "Saç kesimi" );
    e1.addSkill( "Fön" );
    e1.addSkill( "Saç kesimi + fön" );
    e1.addSkill( "Sakal traşı" );
    e1.addSkill( "Saç + sakal paket" );
    e1.addSkill( "Çocuk saç kesimi" );
    e1.addSkill( "Saç boyama" );
    e1.addSkill( "Keratin bakımı" );
    e1.addSkill( "Saç bakım maskesi" );
    e1.addSkill( "Saç yıkama + şekil" );
    e1.addAvailability( slotFor( day, 10, 0, 4 * 60 ) );

    Employee e2( "Merve Fönyapar", "0500 000 0002" );
    e2.addSkill( "Saç kesimi" );
    e2.addSkill( "Fön" );
    e2.addSkill( "Saç kesimi + fön" );
    e2.addSkill( "Sakal traşı" );
    e2.addSkill( "Saç + sakal paket" );
    e2.addSkill( "Çocuk saç kesimi" );
    e2.addSkill( "Saç boyama" );
    e2.addSkill( "Keratin bakımı" );
    e2.addSkill( "Saç bakım maskesi" );
    e2.addSkill( "Saç yıkama + şekil" );
    e2.addAvailability( slotFor( day, 12, 0, 6 * 60 ) );

    serdivanStore.addEmployee( e1 );
    serdivanStore.addEmployee( e2 );

    serdivanStore.addService( Service( "Saç kesimi",           30, 250 ) );
    serdivanStore.addService( Service( "Fön",                  15, 120 ) );
    serdivanStore.addService( Service( "Saç kesimi + fön",     40, 300 ) );
    serdivanStore.addService( Service( "Sakal traşı",          20, 150 ) );
    serdivanStore.addService( Service( "Saç + sakal paket",    45, 380 ) );
    serdivanStore.addService( Service( "Çocuk saç kesimi",     20, 200 ) );
    serdivanStore.addService( Service( "Saç boyama",           60, 600 ) );
    serdivanStore.addService( Service( "Keratin bakımı",       90, 1200 ) );
    serdivanStore.addService( Service( "Saç bakım maskesi",    25, 250 ) );
    serdivanStore.addService( Service( "Saç yıkama + şekil",   15, 120 ) );

    // İkinci şube: farklı mesai ve uzmanlıklar
    Salon arifiyeStore( "Arifiye Şubesi" );
    arifiyeStore.setWorkingHours( slotFor( day, 8, 30, 10 * 60 ) ); // 08:30 - 18:30

    Employee e3( "Burak Bıyıkkeser", "0500 000 0003" );
    e3.addSkill( "Saç kesimi" );
    e3.addSkill( "Fön" );
    e3.addSkill( "Saç kesimi + fön" );
    e3.addSkill( "Sakal traşı" );
    e3.addSkill( "Saç + sakal paket" );
    e3.addSkill( "Çocuk saç kesimi" );
    e3.addSkill( "Saç boyama" );
    e3.addSkill( "Keratin bakımı" );
    e3.addSkill( "Saç bakım maskesi" );
    e3.addSkill( "Saç yıkama + şekil" );

    e3.addAvailability( slotFor( day, 9, 0, 5 * 60 ) );

    Employee e4( "Elif Keratinler", "0500 000 0004" );
    
    e4.addSkill( "Saç kesimi" );
    e4.addSkill( "Fön" );
    e4.addSkill( "Saç kesimi + fön" );
    e4.addSkill( "Sakal traşı" );
    e4.addSkill( "Saç + sakal paket" );
    e4.addSkill( "Çocuk saç kesimi" );
    e4.addSkill( "Saç boyama" );
    e4.addSkill( "Keratin bakımı" );
    e4.addSkill( "Saç bakım maskesi" );
    e4.addSkill( "Saç yıkama + şekil" );

    e4.addAvailability( slotFor( day, 13, 0, 5 * 60 ) );

    arifiyeStore.addEmployee( e3 );
    arifiyeStore.addEmployee( e4 );

    arifiyeStore.addService( Service( "Saç kesimi",           30, 250 ) );
    arifiyeStore.addService( Service( "Fön",                  15, 120 ) );
    arifiyeStore.addService( Service( "Saç kesimi + fön",     40, 300 ) );
    arifiyeStore.addService( Service( "Sakal traşı",          20, 150 ) );
    arifiyeStore.addService( Service( "Saç + sakal paket",    45, 380 ) );
    arifiyeStore.addService( Service( "Çocuk saç kesimi",     20, 200 ) );
    arifiyeStore.addService( Service( "Saç boyama",           60, 600 ) );
    arifiyeStore.addService( Service( "Keratin bakımı",       90, 1200 ) );
    arifiyeStore.addService( Service( "Saç bakım maskesi",    25, 250 ) );
    arifiyeStore.addService( Service( "Saç yıkama + şekil",   15, 120 ) );

    m_salons.push_back( serdivanStore );
    m_salons.push_back( arifiyeStore );

    m_activeSalon = 0;

    m_customers.clear();
    m_customers.emplace_back( "Tarık Yakışıklıoğlu", "0555 555 55 55" );
}
