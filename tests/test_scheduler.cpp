#include "core/Scheduler.h"
#include <cassert>

int main() {
    Scheduler scheduler;

    Salon salon("Test");
    // Çalışma saatleri: 00:00 - 08:00
    salon.setWorkingHours(TimeSlot{0, 8 * 60});

    Customer customer("Ali", "0500 000 0000");

    Employee barber("Mehmet", "0500 000 0001");
    barber.addSkill("Saç kesimi");
    // 01:00 - 03:00 arasında uygun
    barber.addAvailability(TimeSlot{60 * 60, 120});

    Service haircut("Saç kesimi", 30, 100.0);
    Service coloring("Boya", 45, 200.0);

    Appointment out;

    // 1) Mesai saati dışı
    auto result = scheduler.createAppointment(salon, customer, barber, haircut,
        TimeSlot{9 * 60 * 60, 30}, out);
    assert(result == Scheduler::CreateResult::OutsideWorkingHours);

    // 2) Çalışan yeteneği yok
    result = scheduler.createAppointment(salon, customer, barber, coloring,
        TimeSlot{60 * 60, 30}, out);
    assert(result == Scheduler::CreateResult::EmployeeLacksSkill);

    // 3) Çalışan uygun değil (uygunluk dilimi dışında)
    result = scheduler.createAppointment(salon, customer, barber, haircut,
        TimeSlot{4 * 60 * 60, 30}, out);
    assert(result == Scheduler::CreateResult::EmployeeNotAvailable);

    // 4) Çakışma
    Appointment existing(&customer, &barber, haircut, TimeSlot{60 * 60, 30});
    const_cast<Appointment&>(existing).approve();
    salon.addAppointment(existing);

    result = scheduler.createAppointment(salon, customer, barber, haircut,
        TimeSlot{60 * 60 + 15 * 60, 30}, out);
    assert(result == Scheduler::CreateResult::Collision);

    // 5) Mutlu yol: uygunluk + mesai içinde ve çakışma yok
    result = scheduler.createAppointment(salon, customer, barber, haircut,
        TimeSlot{2 * 60 * 60, 30}, out);
    assert(result == Scheduler::CreateResult::Ok);
    assert(out.getStatus() == Appointment::Status::Approved);
    assert(!salon.getAppointments().empty());

    return 0;
}
