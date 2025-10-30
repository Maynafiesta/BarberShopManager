// lib/include/storage/IRepository.h
#pragma once
#include <vector>
#include "model/Salon.h"

class IRepository {
public:
    virtual ~IRepository() = default;
    virtual void saveSalon(const Salon& salon) = 0;
    virtual std::vector<Salon> loadAllSalons() const = 0;
};
