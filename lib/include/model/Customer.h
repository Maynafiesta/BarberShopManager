#pragma once
#include "Person.h"

class Customer : public Person {
public:
    Customer(std::string name, std::string phone)
        : Person(std::move(name), std::move(phone), Role::Customer) {}

    std::string summary() const override {
        return "Customer: " + m_name + " (" + m_phone + ")";
    }
};
