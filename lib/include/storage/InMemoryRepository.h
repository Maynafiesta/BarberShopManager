#pragma once
#include "IRepository.h"

class InMemoryRepository : public IRepository {
public:
    void saveSalon(const Salon& salon) override;
    std::vector<Salon> loadAllSalons() const override;

private:
    std::vector<Salon> m_salons;
};
