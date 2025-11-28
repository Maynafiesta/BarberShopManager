// lib/include/model/Employee.h
#pragma once
#include <vector>
#include <string>
#include "model/Person.h"
#include "model/TimeSlot.h"

class Employee : public Person {
public:
    Employee(std::string name, std::string phone)
        : Person(std::move(name), std::move(phone), Role::Employee) {}

    void addSkill(const std::string& s) { m_skills.push_back(s); }
    bool hasSkill(const std::string& s) const;
    const std::vector<std::string>& getSkills() const noexcept { return m_skills; }

    void addAvailability(const TimeSlot& t) { m_available.push_back(t); }
    void clearAvailability() { m_available.clear(); }
    const std::vector<TimeSlot>& getAvailability() const noexcept { return m_available; }

    std::string summary() const override { return "Employee: " + m_name; }

private:
    std::vector<std::string> m_skills;
    std::vector<TimeSlot>    m_available;
};
