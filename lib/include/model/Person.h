// lib/include/model/Person.h
#pragma once
#include <string>
#include "model/Role.h"

class Person {
public:
    Person(std::string name, std::string phone, Role role)
        : m_name(std::move(name)), m_phone(std::move(phone)), m_role(role) {}
    virtual ~Person() = default;

    const std::string& getName() const noexcept { return m_name; }
    void setName(const std::string& n) { m_name = n; }

    const std::string& getPhone() const noexcept { return m_phone; }
    void setPhone(const std::string& p) { m_phone = p; }

    Role getRole() const noexcept { return m_role; }
    virtual std::string summary() const = 0;

protected:
    std::string m_name;
    std::string m_phone;
    Role        m_role;
};
