# core business logic (Qt bağımsız)

set(BARBER_CORE_SOURCES
  src/Person.cpp
  src/Customer.cpp
  src/Employee.cpp
  src/TimeSlot.cpp
  src/Service.cpp
  src/Appointment.cpp
  src/Salon.cpp
  src/Scheduler.cpp
  src/InMemoryRepository.cpp
)

add_library(barber_core ${BARBER_CORE_SOURCES})

target_include_directories(barber_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(barber_core PUBLIC cxx_std_17)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(barber_core PRIVATE -Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(barber_core PRIVATE /W4)
endif()
