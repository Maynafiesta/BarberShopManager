#pragma once
#include <vector>
#include "Salon.h"

class IRepository {
public:
    virtual ~IRepository() = default;

    virtual void saveSalon(const Salon& salon) = 0;
    virtual std::vector<Salon> loadAllSalons() const = 0;
};

