# lib/CMakeLists.txt

# Bu kütüphanenin kaynak dosyalarını listele.
# Şimdilik örnek cpp dosyaları yazdım. Sen hangilerini oluşturursan buraya ekle.
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

# Public header path'i export et
target_include_directories(barber_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Bu kütüphane de C++17 kullansın
target_compile_features(barber_core PUBLIC cxx_std_17)

# İstersen buraya sadece core'a özel derleme seçenekleri de ekleyebilirsin
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(barber_core PRIVATE -Wall -Wextra -Wpedantic)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(barber_core PRIVATE /W4)
endif()
