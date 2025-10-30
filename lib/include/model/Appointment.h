// lib/include/model/Appointment.h
#pragma once
#include "model/Customer.h"
#include "model/Employee.h"
#include "model/Service.h"
#include "model/TimeSlot.h"

class Appointment {
public:
    enum class Status { Pending, Approved, Rejected };

    Appointment(const Customer* c = nullptr,
                const Employee* e = nullptr,
                const Service& s = {},
                const TimeSlot& t = {})
        : m_customer(c), m_employee(e), m_service(s), m_slot(t), m_status(Status::Pending) {}

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
