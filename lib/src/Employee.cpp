// lib/src/Employee.cpp
#include "model/Employee.h"

#include <algorithm>

bool Employee::hasSkill(const std::string& s) const {
    return std::find(m_skills.begin(), m_skills.end(), s) != m_skills.end();
}
