#include <iostream>
#include "Car.hpp"
#include "Motorcycle.hpp"
#include "CorporateCustomer.hpp"
#include "IndividualCustomer.hpp"

int main() 
{
    // Araçlar oluşturuluyor
    Vehicle* car1 = new Car( "Toyota Corolla", 2020, 50 );
    Vehicle* motorcycle1 = new Motorcycle( "Yamaha R1", 2022, 40 );

    // Müşteriler oluşturuluyor
    Customer* customer1 = new CorporateCustomer( "John Doe", "john@company.com", "TechCorp" );
    Customer* customer2 = new IndividualCustomer( "Jane Doe", "jane@personal.com", true );

    // Araçları ve Müşterileri görüntüle
    std::cout << "\nAvailable vehicles for rent:\n";
    car1->displayDetails();
    motorcycle1->displayDetails();

    std::cout << "\nCustomer Details:\n";
    customer1->displayDetails();
    customer2->displayDetails();

    // İndirimli fiyatları hesapla
    double rentalPriceCar = car1->calculateRentalPrice( 5 );
    double rentalPriceMotorcycle = motorcycle1->calculateRentalPrice( 5 );
    
    std::cout << "\nRental Prices for 5 days:\n";
    std::cout << "Car: " << rentalPriceCar * ( 1 - customer1->getDiscount() ) << std::endl;
    std::cout << "Motorcycle: " << rentalPriceMotorcycle * ( 1 - customer2->getDiscount() ) << std::endl;

    // Dinamik bellek temizliği
    delete car1;
    delete motorcycle1;
    delete customer1;
    delete customer2;

    return 0;
}
