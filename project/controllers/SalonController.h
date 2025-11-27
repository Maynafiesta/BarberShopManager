// project/controllers/SalonController.h
#pragma once
#include <QDate>
#include <vector>
#include "model/Salon.h"
#include "model/Customer.h"

class SalonController {
public:
    SalonController(std::vector<Salon>& salons, std::vector<Customer>& customers)
        : m_salons(salons), m_customers(customers) {}

    void loadDemoData(const QDate& day);

    bool setActiveSalon(size_t idx);
    size_t activeSalonIndex() const noexcept { return m_activeSalon; }

    Salon*       currentSalon();
    const Salon* currentSalon() const;

    const std::vector<Employee>& employees() const;
    const std::vector<Service>&  services()  const;
    const std::vector<Appointment>& appointments() const;

    const std::vector<Salon>& salons() const noexcept { return m_salons; }

    bool rejectAppointmentAt(size_t idx);
    bool removeAppointmentAt(size_t idx);

private:
    Salon& active();
    const Salon& active() const;

    std::vector<Salon>&     m_salons;
    std::vector<Customer>&  m_customers;
    size_t                  m_activeSalon{0};
};
