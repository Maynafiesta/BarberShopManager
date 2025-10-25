#pragma once
#include <string>

class Service {
public:
    Service(std::string name, int durationMinutes, double price)
        : m_name(std::move(name)),
          m_durationMinutes(durationMinutes),
          m_price(price) {}

    const std::string& getName() const noexcept { return m_name; }
    int  getDurationMinutes() const noexcept { return m_durationMinutes; }
    double getPrice() const noexcept { return m_price; }

private:
    std::string m_name;   // "Saç kesimi"
    int         m_durationMinutes;
    double      m_price;
};
