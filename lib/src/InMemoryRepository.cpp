// lib/src/InMemoryRepository.cpp
#include "storage/InMemoryRepository.h"

void InMemoryRepository::saveSalon(const Salon& s) { m_salons.push_back(s); }
std::vector<Salon> InMemoryRepository::loadAllSalons() const { return m_salons; }
