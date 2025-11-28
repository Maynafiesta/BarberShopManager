// lib/src/Employee.cpp
#include "model/Employee.h"

#include <algorithm>

void Employee::setSkills(std::vector<std::string> skills) {
    std::sort(skills.begin(), skills.end());
    skills.erase(std::unique(skills.begin(), skills.end()), skills.end());
    m_skills = std::move(skills);
}

bool Employee::hasSkill(const std::string& s) const {
    return std::find(m_skills.begin(), m_skills.end(), s) != m_skills.end();
}
