#pragma once
#include <vector>
#include <string>
#include "Person.h"
#include "TimeSlot.h"

class Employee : public Person {
public:
    Employee(std::string name, std::string phone)
        : Person(std::move(name), std::move(phone), Role::Employee) {}

    void addSkill(const std::string& serviceName) {
        m_skills.push_back(serviceName);
    }

    const std::vector<std::string>& getSkills() const noexcept {
        return m_skills;
    }

    void addAvailability(const TimeSlot& slot) {
        m_availableSlots.push_back(slot);
    }

    const std::vector<TimeSlot>& getAvailability() const noexcept {
        return m_availableSlots;
    }

    std::string summary() const override {
        return "Employee: " + m_name;
    }

private:
    std::vector<std::string> m_skills;        // "Saç kesimi", "Boya", "Sakal tıraşı"
    std::vector<TimeSlot>    m_availableSlots;
};
