#pragma once
#include <string>
#include "Role.h"

class Person {
public:
    Person(std::string name, std::string phone, Role role)
        : m_name(std::move(name)), m_phone(std::move(phone)), m_role(role) {}

    virtual ~Person() = default;

    const std::string& getName() const noexcept { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    const std::string& getPhone() const noexcept { return m_phone; }
    void setPhone(const std::string& phone) { m_phone = phone; }

    Role getRole() const noexcept { return m_role; }

    // Polimorfizm: her kişi için kısa bilgi
    virtual std::string summary() const = 0;

protected:
    std::string m_name;
    std::string m_phone;
    Role        m_role;
};
