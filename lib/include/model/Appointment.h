#pragma once
#include <string>
#include "Customer.h"
#include "Employee.h"
#include "Service.h"
#include "TimeSlot.h"

class Appointment {
public:
    enum class Status {
        Pending,
        Approved,
        Rejected
    };

    Appointment(const Customer* customer,
                const Employee* employee,
                const Service& service,
                const TimeSlot& slot)
        : m_customer(customer),
          m_employee(employee),
          m_service(service),
          m_slot(slot),
          m_status(Status::Pending) {}

    const Customer* getCustomer() const noexcept { return m_customer; }
    const Employee* getEmployee() const noexcept { return m_employee; }
    const Service&  getService()  const noexcept { return m_service; }
    const TimeSlot& getSlot()     const noexcept { return m_slot; }

    Status getStatus() const noexcept { return m_status; }
    void approve() { m_status = Status::Approved; }
    void reject()  { m_status = Status::Rejected; }

    double getTotalPrice() const { return m_service.getPrice(); }

private:
    const Customer* m_customer;
    const Employee* m_employee;
    Service         m_service;
    TimeSlot        m_slot;
    Status          m_status;
};
